/*
 * Copyright (C) 2018 Universitat Autonoma de Barcelona 
 * David Castells-Rufas <david.castells@uab.cat>
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   HOGProcessor.cpp
 * Author: dcr
 * 
 * Created on February 10, 2018, 11:21 AM
 */

#include "HOGProcessor.h"
#include "Image.h"
#include "BufferedImage.h"
#include "ReferenceSubImage.h"

#include <math.h>
#include <vector>

HOGProcessor::HOGProcessor()
{
    m_cellWidth = 8;
    m_cellHeight = 8;
    m_blockWidth = 2;
    m_blockHeight = 2;
}

/**
 * HOG Processor
 * 
 * @param cellWidth cell width in pixels
 * @param cellHeight cell height in pixels
 * @param blockWidth block width in cells
 * @param blockHeight block height in cells
 */
HOGProcessor::HOGProcessor(int cellWidth, int cellHeight, int blockWidth, int blockHeight)
{
    m_cellWidth = cellWidth;
    m_cellHeight = cellHeight;
    m_blockWidth = blockWidth;
    m_blockHeight = blockHeight;
}



HOGProcessor::~HOGProcessor()
{
}

/**
 * Create a set of images to visualize the HOG features from input images
 * @param images
 * @return 
 */
std::vector<Image*> HOGProcessor::createHogImages(std::vector<Image*> images)
{
    std::vector<Image*> ret;
    
    for (int i=0; i<images.size(); i++)
    {
        Image* person = images.at(i);
        HOGFeature* feature = createFeature(person);
        Image* hogImage = createHOGVisualization(feature);
        hogImage->m_objId = person->m_objId;    // copy the object ID for correct naming
        delete feature;
        ret.push_back(hogImage);
    }
    return ret;
}

/**
 * Create HOG features from a set of input images
 * @param images
 * @return 
 */
std::vector<HOGFeature*> HOGProcessor::createHogFeatures(std::vector<Image*> images)
{
    std::vector<HOGFeature*> ret;
    
    for (int i=0; i<images.size(); i++)
    {
        Image* person = images.at(i);
        HOGFeature* feature = createFeature(person);
        
        ret.push_back(feature);
    }
    return ret;
}

/**
 * Create a HOG feature from an Image
 * @todo handle color channels
 * @param image
 * @return 
 * 
 */
HOGFeature* HOGProcessor::createFeature(Image* image)
{
    HOGFeature* feature = new HOGFeature(image->m_width, image->m_height, m_cellWidth, m_cellHeight, m_blockWidth, m_blockHeight, 3);
    
    #pragma omp parallel for
    for (int colorChannel = 0; colorChannel < 3; colorChannel++)
    {
        for (int yblock=0; yblock < feature->m_numBlocksY; yblock++)
            for (int xblock=0; xblock < feature->m_numBlocksX; xblock++)
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {
                        // gradients are short (not char) because we need 10 bits 
                        short gradientx[8*8];
                        short gradienty[8*8];
                        
                        computeGradient(image, feature, xblock, yblock, xcell, ycell, colorChannel, gradientx, gradienty);

                        // clear the histogram bins for this cell
                        unsigned int* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);

                        for (int bin = 0; bin < 9; bin++)
                            pBin[bin] = 0;

                        computeHistogram(gradientx, gradienty, pBin);
                    }
    }
    
    feature->m_objId = image->m_objId;    // copy the object ID for correct naming
    
    return feature;
}


/**
 * @todo fix the border cases (they should contribute to the other extreme)
 * @param pBin
 */
void HOGProcessor::computeHistogram(short* gradientx, short* gradienty, unsigned int* pBin)
{
    double pi = 3.14159265359;
    double pi18 = pi / 18.0;

    // Now compute magnitude and orientation (rectangular to polar coordinates)
    for (int y=0; y < 8; y++)
        for (int x=0; x < 8; x++)
        {
            double mag = sqrt((gradientx[x+y*8] * gradientx[x+y*8]) + (gradienty[x+y*8] * gradienty[x+y*8]));
            double orientation = atan2(gradienty[x+y*8], gradientx[x+y*8]);

            if (mag == 0)
                    // avoid wasting time for mag = 0
                    continue;

            if (orientation < 0)
                    orientation = pi - orientation;
            if (orientation > pi)
                    orientation = orientation - pi;

            //printf("mag: %d ori: %f\n", mag, orientation);

            // now select the bins where we should aggregate the values


            int bin0 = -1;	// -1 means no bin is used
            int bin1 = -1;	// 

            bin0 = (orientation - pi18) / (2*pi18);		// bin0 will automanically be negative for orientation < pi18
            bin1 = bin0 + 1;



            if (orientation < pi18)
            {
                    bin0 = -1;
                    bin1 = 0;
            }

            double bin0center = (bin0 * (2*pi18)) + pi18;
            double bin1center = (bin1 * (2*pi18)) + pi18;

            //printf("bin0 center: %f bin1 center: %f\n", bin0center, bin1center);

            if (bin1 > 8)
                bin1 = -1;

            // weight 0 is computed as the factor of the distance from bin1 center with the bin width
            double w0 = (bin1center - orientation) / (2*pi18);
            // weight 1 is computed as the factor of the distance from bin0 center with the bin width
            double w1 = (orientation - bin0center) / (2*pi18);

            //if (bin0 >= 0)
                    //printf("bin0. bin[%d] = %d * %f = %d\n", bin0, mag, w0, (int)(mag*w0));
            //if (bin1 >= 0)
                    //printf("bin1. bin[%d] = %d * %f = %d\n", bin1, mag, w1, (int)(mag*w1));

            // Finally add the computed values if bins are valid
            if (bin0 >= 0)
                    pBin[bin0] += mag * w0;
            if (bin1 >= 0)
                    pBin[bin1] += mag * w1;
        }    
}



/**
 * 
 * @param image
 * @param feature
 * @param xcell
 * @param ycell
 * @param colorChannel
 */
void HOGProcessor::computeGradient(Image* image, HOGFeature* feature, int xblock, int yblock, int xcell, int ycell, int colorChannel, short* gradientx, short* gradienty)
{
    // compute the gradients 
    // [-1, 0, 1] for horizontal and vertical gradients
    for (int y=0; y < 8; y++)
        for (int x=0; x < 8; x++)
        {
            int fx = feature->getImageXFromCellIndexX(xblock, xcell) + x;
            int fy = feature->getImageYFromCellIndexY(yblock, ycell) + y;

            int fx_m1 = (x > 0)? (fx-1) : fx;
            int fx_p1 = (fx < (image->m_width-1))? (fx+1) : fx;
            int fy_m1 = (y > 0)? (fy-1) : fy;
            int fy_p1 = (fy < (image->m_width-1))? (fy+1) : fy;

            gradientx[x+y*8] = -image->get(fx_m1, fy, colorChannel) + image->get(fx_p1, fy, colorChannel);
            gradienty[x+y*8] = -image->get(fx, fy_m1, colorChannel) + image->get(fx, fy_p1, colorChannel);
        }
    
}

/**
 * Build a visualization of the HOG feature, we ignore the overlapped cells
 * @todo what to display for cells that have multiple contributions from different blocks?
 * @param feature
 * @return 
 */
Image* HOGProcessor::createHOGVisualization(HOGFeature* feature)
{
    BufferedImage* image = new BufferedImage(feature->m_imageWidth, feature->m_imageHeight);

    const double pi = 3.14159265359;
    const double pi18 = pi / 18.0;
    const double pi9 = pi / 9.0;
    const double pi2 = pi / 2.0;

    double maxNormalization = 0;
    
    // if global normalization
    for (int colorChannel =0 ; colorChannel < 3; colorChannel++)
        for (int yblock=0; yblock < feature->getBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < feature->getBlocksInAxisX(); xblock++)
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {    
                        unsigned int* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);
                        for (int i=0; i < 9; i++)
                        {
                            if (pBin[i] > maxNormalization)
                                maxNormalization = pBin[i];
                        }
                    }    
    
    maxNormalization = 255 / maxNormalization;
    
    for (int colorChannel =0 ; colorChannel < 3; colorChannel++)
        for (int yblock=0; yblock < feature->getBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < feature->getBlocksInAxisX(); xblock++)
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {    
                        unsigned int* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);

                        ReferenceSubImage window(image, feature->getImageXFromCellIndexX(xblock, xcell), feature->getImageYFromCellIndexY(yblock, ycell), 8, 8);

                        double angle = pi18;
                        for (int i=0; i < 9; i++)
                        {
                            if (m_rotateHog)
                                window.drawAngleLine(angle+pi2, pBin[i] * maxNormalization, colorChannel);
                            else
                                window.drawAngleLine(angle, pBin[i] * maxNormalization, colorChannel);
                            angle += pi9;
                        }
                    }
    
    return image;
}
