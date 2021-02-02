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

#include "FPGAHOGProcessor.h"
#include "Image.h"
#include "BufferedImage.h"
#include "ReferenceSubImage.h"
#include <assert.h>
#include <math.h>
#include <vector>

FPGAHOGProcessor::FPGAHOGProcessor()
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
FPGAHOGProcessor::FPGAHOGProcessor(int cellWidth, int cellHeight, int blockWidth, int blockHeight)
{
    m_cellWidth = cellWidth;
    m_cellHeight = cellHeight;
    m_blockWidth = blockWidth;
    m_blockHeight = blockHeight;

}



FPGAHOGProcessor::~FPGAHOGProcessor()
{

}


HOGFeature* FPGAHOGProcessor::createFeature(Image* image)
{
    HOGFeature* ret = allocateMemFeature(image);
    HOGFeature* preNorm = allocateMemFeature(image);
    double* mag = allocateMemGradient(image);
    double* orient = allocateMemGradient(image);
    
    computeGradient(image,  mag, orient);
    computeHistogram(image, mag, orient, preNorm);
    computeNorm(image, preNorm, ret);
    
    delete mag;
    delete orient;
    delete preNorm;
    
    return ret;
}


/**
 * Create a set of images to visualize the HOG features from input images
 * @param images
 * @return 
 */
//std::vector<Image*> FPGAHOGProcessor::createHogImages(std::vector<Image*> images)
//{
//    std::vector<Image*> ret;
//    
//    for (int i=0; i<images.size(); i++)
//    {
//        Image* person = images.at(i);
//        HOGFeature* feature = createFeature(person);
//        Image* hogImage = createHOGVisualization(feature);
//        hogImage->m_objId = person->m_objId;    // copy the object ID for correct naming
//        delete feature;
//        ret.push_back(hogImage);
//    }
//    return ret;
//}

/**
 * Create HOG features from a set of input images
 * @param images
 * @return 
 */
//std::vector<HOGFeature*> FPGAHOGProcessor::createHogFeatures(std::vector<Image*> images)
//{
//    std::vector<HOGFeature*> ret;
//    
//    for (int i=0; i<images.size(); i++)
//    {
//        Image* person = images.at(i);
//        HOGFeature* feature = createFeature(person);
//        
//        ret.push_back(feature);
//    }
//    return ret;
//}


/**
 * Preallocates the memory for the intermediate memories, which depend on the image size
 * @param image
 */
HOGFeature* FPGAHOGProcessor::allocateMemFeature(Image* image)
{
    int numCellsX = image->m_width / m_cellWidth;
    int numCellsY = image->m_height / m_cellHeight;

    int blocksInXAxis = numCellsX / m_blockWidth;  // how many blocks fit in image width
    int blocksInYAxis = numCellsY / m_blockHeight; // how many blocks fit in image height
    
    int numBlocksX = blocksInXAxis * 2 - 1;
    int numBlocksY = blocksInYAxis * 2 - 1;
    
    assert(numBlocksX > 2);
    assert(numBlocksY > 2);
    
//    m_numCellsX = (imageWidth / cellWidth) * 2 -1;
//    m_numCellsY = (imageHeight / cellHeight) * 2 -1;
        
    
    return new HOGFeature(image->m_width, image->m_height, m_cellWidth, m_cellHeight, m_blockWidth, m_blockHeight, image->m_channels);
}


double* FPGAHOGProcessor::allocateMemGradient(Image* image)
{
    
    int numCellsX = image->m_width / m_cellWidth;
    int numCellsY = image->m_height / m_cellHeight;
    return new double[image->m_channels*image->m_width*image->m_height];
}

void FPGAHOGProcessor::computeGradient(Image* image, double* pGradientMag, double* pGradientOrient)
{
    double pi = M_PI;
    
//    printf("grad\n");
    
    // compute gradient and bins for all the image, here we do not need to work with blocks
    int numCellsX = image->m_width / m_cellWidth;
    int numCellsY = image->m_height / m_cellHeight;
        
    //double rawBins[image->m_channels][numCellsX][numCellsY][9];
    
    for (int colorChannel = 0; colorChannel < image->m_channels; colorChannel++)
    {
        //#pragma omp parallel for
        for (int fy=0; fy < image->m_height ; fy++)
            for (int fx = 0; fx < image->m_width; fx++)
            {
                // consider corner cases
                int fx_m1 = (fx > 0)? (fx-1) : fx;
                int fx_p1 = (fx < (image->m_width-1))? (fx+1) : fx;
                int fy_m1 = (fy > 0)? (fy-1) : fy;
                int fy_p1 = (fy < (image->m_height-1))? (fy+1) : fy;

                int gradientx = -image->get(fx_m1, fy, colorChannel) + image->get(fx_p1, fy, colorChannel);
                int gradienty = -image->get(fx, fy_m1, colorChannel) + image->get(fx, fy_p1, colorChannel);

                double mag = sqrt((gradientx * gradientx) + (gradienty * gradienty));
                double orientation = atan2(gradienty, gradientx);

//            if (mag == 0)
//                // avoid wasting time for mag = 0
//                continue;
                
                // save orientation in the 0 - pi range
                if (orientation < 0)
                    orientation = pi + orientation;
                if (orientation > pi)
                    orientation = orientation - pi;
                
                assert(orientation >= 0 && orientation <= M_PI);

                pGradientMag[colorChannel*image->m_height*image->m_width+fy*image->m_width+fx] = mag;
                pGradientOrient[colorChannel*image->m_height*image->m_width+fy*image->m_width+fx] = orientation;                
            }
    }
    
}


void FPGAHOGProcessor::computeHistogram(Image* image, double* pGradientMag, double* pGradientOrient, HOGFeature* preNorm)
{
//    printf("his\n");
    
    // compute gradient and bins for all the image, here we do not need to work with blocks
    int numCellsX = image->m_width / m_cellWidth;
    int numCellsY = image->m_height / m_cellHeight;
        
    //double rawBins[image->m_channels][numCellsX][numCellsY][9];
    
    for (int colorChannel =0 ; colorChannel < preNorm->m_colorChannels; colorChannel++)
        for (int yblock=0; yblock < preNorm->getOverlappingBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < preNorm->getOverlappingBlocksInAxisX(); xblock++)
            {
                // bsx,bsy are image coordinates 
                int bsx = preNorm->getImageXFromCellIndexX(xblock, 0);
                int bsy = preNorm->getImageXFromCellIndexX(yblock, 0);

                for (int ycell=0; ycell < preNorm->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < preNorm->m_blockWidth; xcell++)
                    {    
                        double* pDstBin = preNorm->getBin(xblock, yblock, xcell, ycell, colorChannel);
                        
                        for (int ycellsrc=0; ycellsrc < preNorm->m_blockHeight; ycellsrc++)
                            for (int xcellsrc = 0; xcellsrc < preNorm->m_blockWidth; xcellsrc++)
                            {
                                for (int cy = 0; cy < preNorm->m_cellHeight; cy++)
                                    for (int cx = 0; cx < preNorm->m_cellWidth; cx++)
                                    {
                                        // fx,fy are image coordinates 
                                        int fx = preNorm->getImageXFromCellIndexX(xblock, xcellsrc) + cx;
                                        int fy = preNorm->getImageXFromCellIndexX(yblock, ycellsrc) + cy;
                                        
                                        // x,y coordinates relative to start of the block
                                        int relbsx = fx-bsx;    
                                        int relbsy = fy-bsy;
                                        
                                        double gMag = pGradientMag[colorChannel*image->m_width*image->m_height+fy*image->m_width+fx];
                                        double gOri = pGradientOrient[colorChannel*image->m_width*image->m_height+fy*image->m_width+fx];
                                        
                                        // la fx fy està malament
                                        double cellW = cellInterpolation(xcell*m_cellWidth+3.5, ycell*m_cellHeight+3.5, relbsx, relbsy);
                                        double cellG = cellGaussian(relbsx, relbsy);
                                        
                                        int bin0, bin1;
                                        double mag0, mag1;
                                        
                                        interpolateBins(gMag, gOri, &bin0, &bin1, &mag0, &mag1);
                                        
                                        pDstBin[bin0] += mag0; // * cellW * cellG;
                                        pDstBin[bin1] += mag1; //  * cellW * cellG;
                                    }
                            }
                    }
            }
    
}

double FPGAHOGProcessor::cellInterpolation(double centerx, double centery, double x, double y)
{
    double d = sqrt(pow(centerx-x, 2)+pow(centery-y,2));
    assert(d > 0.1);
    return 1/d;
}
                 
double FPGAHOGProcessor::cellGaussian(double x, double y)
{
    double k = 1/(2*M_PI*8*8);
    double q = pow(x-7.5, 2) + pow(y-7.5, 2);
    return k * exp(-q*k);
}


void FPGAHOGProcessor::interpolateBins(double gMag, double gOri, int* bin0, int* bin1, double* mag0, double* mag1)
{
    double pi18 = M_PI / 18;
    double pi9 = M_PI / 9;
    
    // this is already checked when storing the orientation
//    if (gOri < 0)
//        gOri = M_PI - gOri;
    
    // Ensure that the orientation is in the 0 - pi range
    assert(gOri >= 0 && gOri <= M_PI);
    
    *bin0 = (M_PI + gOri - pi18) / pi9;
    *bin0 = *bin0 % 9;
    *bin1 = (*bin0 + 1) % 9; 
    
    double bc0 = (*bin0) * pi9 + pi18;
    //double bc1 = (*bin1) * pi9 + pi18;
    
    
    double dbc0 = (gOri <= bc0)? M_PI + gOri - bc0 : gOri - bc0;
    
    double w0 = dbc0 / pi9; //  : bc0 + M_PI - gOri;
    double w1 = 1 - w0;
    
    *mag0 = gMag * w0;
    *mag1 = gMag * w1;
}

/**
 * 
 * @param image
 * @param feature
 */
void FPGAHOGProcessor::computeNorm(Image* image, HOGFeature* preNorm, HOGFeature* feature)
{
    // compute gradient and bins for all the image, here we do not need to work with blocks
    int numCellsX = image->m_width / m_cellWidth;
    int numCellsY = image->m_height / m_cellHeight;
        
//    printf("norm\n");
    
    // Now compute bin normalization
    for (int colorChannel = 0; colorChannel < image->m_channels; colorChannel++)
    {
        //#pragma omp parallel for
        for (int yblock=0; yblock < feature->getOverlappingBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < feature->getOverlappingBlocksInAxisX(); xblock++)
            {
                // Normalize Using L1-Sqrt
                double L1_norm = 1;
                
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {
//                        int cellx = xblock + xcell;
//                        int celly = yblock + ycell;

                        double* pSrcBin = preNorm->getBin(xblock, yblock, xcell, ycell, colorChannel);

                        for (int bin=0; bin < 9; bin++)
                        {
                            
                            L1_norm += pSrcBin[bin];
                        }
                    }
                
                double L1_sqrt = sqrt(L1_norm);
                
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {
//                        int cellx = xblock + xcell;
//                        int celly = yblock + ycell;
                            
                        double* pSrcBin = preNorm->getBin(xblock, yblock, xcell, ycell, colorChannel);
                        double* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);
                        
                        for (int bin=0; bin < 9; bin++)
                        {
                            pBin[bin] = pSrcBin[bin] / L1_sqrt;
                        }
                    }
            }
    }
    
    feature->m_objId = image->m_objId;    // copy the object ID for correct naming
    
}




/**
 * Build a visualization of the HOG feature, we ignore the overlapped cells
 * @todo what to display for cells that have multiple contributions from different blocks?
 * @param feature
 * @return 
 */
Image* FPGAHOGProcessor::createHOGVisualization(HOGFeature* feature)
{
    BufferedImage* image = new BufferedImage(feature->m_imageWidth, feature->m_imageHeight);

    const double pi = 3.14159265359;
    const double pi18 = pi / 18.0;
    const double pi9 = pi / 9.0;
    const double pi2 = pi / 2.0;

    double maxNormalization = 0;
    
    // if global normalization
    for (int colorChannel =0 ; colorChannel < feature->m_colorChannels; colorChannel++)
        for (int yblock=0; yblock < feature->getBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < feature->getBlocksInAxisX(); xblock++)
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {    
                        double* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);
                        for (int i=0; i < 9; i++)
                        {
                            if (pBin[i] > maxNormalization)
                                maxNormalization = pBin[i];
                        }
                    }    
    
    maxNormalization = 255 / maxNormalization;
    
    for (int colorChannel =0 ; colorChannel < feature->m_colorChannels; colorChannel++)
        for (int yblock=0; yblock < feature->getBlocksInAxisY(); yblock++)
            for (int xblock=0; xblock < feature->getBlocksInAxisX(); xblock++)
                for (int ycell=0; ycell < feature->m_blockHeight; ycell++)
                    for (int xcell = 0; xcell < feature->m_blockWidth; xcell++)
                    {    
                        double* pBin = feature->getBin(xblock, yblock, xcell, ycell, colorChannel);

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
