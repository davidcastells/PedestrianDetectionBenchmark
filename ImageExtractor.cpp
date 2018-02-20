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
 * File:   ImageExtractor.cpp
 * Author: dcr
 * 
 * Created on February 9, 2018, 6:14 PM
 */

#include "ImageExtractor.h"
#include "File.h"
#include "ReferenceSubImage.h"
#include "BufferedImage.h"
#include "HOGFeature.h"
#include "BoundingBox.h"

#include <png.h>
#include <stdlib.h>

ImageExtractor::ImageExtractor()
{
}

ImageExtractor::ImageExtractor(const ImageExtractor& orig)
{
}

ImageExtractor::~ImageExtractor()
{
}

void ImageExtractor::setFrameNumber(int n)
{
    m_frameNumber = n;
}

void ImageExtractor::setExtractionPath(const char* path)
{
    File file(path);
    
    if (!file.exists())
        file.mkdirs();
    
    m_extractionPath = (char*) path;
}

void ImageExtractor::setImage(Image* image)
{
    m_image = image;
}

std::string format(const char *fmt, ...);

void ImageExtractor::extractFrame()
{
    std::string outFile = format("%s/frame%d.png", m_extractionPath.c_str() , m_frameNumber);
    saveImageAsPng(outFile.c_str(), m_image);
}

BoundingBox ImageExtractor::createRandomBox(int w, int h, int boxx, int boxy)
{
    BoundingBox b(0, 0, 0, 0, false);
    
    int maxx = w - boxx;
    int maxy = h - boxy;
    
    b.m_width = boxx;
    b.m_height = boxy;
    
    b.m_x = rand() % maxx;
    b.m_y = rand() % maxy;
    
    return b;
}

std::vector<Image*> ImageExtractor::getNonPersons(std::vector<BoundingBox> boxes)
{
    std::vector<Image*> ret;
    int imagesToExtract = boxes.size();
    int imagesExtracted = 0;
    Image* image;
    
    do
    {
        // create a random box
        BoundingBox box = createRandomBox(m_image->m_width, m_image->m_height, m_resizeX, m_resizeY);
        
        if (!box.colide(boxes))
        {
            image = new ReferenceSubImage(m_image, &box);
            
            ret.push_back(image);
            imagesExtracted++;
        }        
    } while (imagesExtracted < imagesToExtract);
    
    return ret;
}

std::vector<Image*> ImageExtractor::getPersonsHigher(std::vector<BoundingBox> boxes, int minHeight)
{
    std::vector<Image*> ret;
    
    for (int i=0; i < boxes.size(); i++)
    {
        BoundingBox box = boxes.at(i);
        
        if (!box.m_occluded)
        {
            if (box.isPerson() && box.m_height > minHeight && box.m_x >= 0 && box.m_y >= 0)
            {
                ReferenceSubImage* img = new ReferenceSubImage(m_image, &box);
                ret.push_back(img);
            }
        }
    }
    
    return ret;
}

std::vector<Image*> ImageExtractor::resizePersons(std::vector<Image*> persons)
{
    std::vector<Image*> ret;
    
    for (int i=0; i < persons.size(); i++)
    {
        Image* image = persons.at(i);
        
        if (m_resize)
        {
            BufferedImage* resized = new BufferedImage(m_resizeX, m_resizeY);
            
            double factor = (double) image->m_height / (double) m_resizeY;
            
            int totalWidth = ((ReferenceSubImage*)image)->m_reference->m_width;
            int totalHeight = ((ReferenceSubImage*)image)->m_reference->m_height;
            
            // Ignore boundary images
            if (((ReferenceSubImage*)image)->m_referenceX + (m_resizeX*factor) > totalWidth)
                continue;
            if (((ReferenceSubImage*)image)->m_referenceY + image->m_height > totalHeight)
                continue;
            

            resized->resizeFrom(image);
            ret.push_back(resized);
        }
    }

    return ret;
}



void ImageExtractor::saveHogFeatures(std::vector<HOGFeature*> persons)
{
    for (int i=0; i < persons.size(); i++)
    {
        HOGFeature* feature = persons.at(i);
        
        std::string outFile = format("%s/frame%d_%s%d_hog.csv", m_extractionPath.c_str(), m_frameNumber, "person", feature->m_objId);
        
        printf("Saving %s..\n", outFile.c_str());
        saveHogFeatureAsCsv(outFile.c_str(), feature);
        
    }
}

/**
 * Save the HOG visualitzation images from persons
 * @param persons
 */
void ImageExtractor::saveHogPersons(std::vector<Image*> persons)
{
    for (int i=0; i < persons.size(); i++)
    {
        Image* image = persons.at(i);
        
        std::string outFile = format("%s/frame%d_%s%d_%shog.png", m_extractionPath.c_str(), 
                m_frameNumber, "person", image->m_objId, (m_rotateHog)? "R":"");
        
        printf("Saving %s..\n", outFile.c_str());
        saveImageAsPng(outFile.c_str(), image);
        
    }
}



/**
 * Saves the extracted persons images as PNG files
 * @param persons
 */
void ImageExtractor::savePersons(std::vector<Image*> persons)
{
    for (int i=0; i < persons.size(); i++)
    {
        Image* image = persons.at(i);
        
        std::string outFile = format("%s/frame%d_%s%d.png", m_extractionPath.c_str(), m_frameNumber, "person", image->m_objId);
        printf("Saving %s..\n", outFile.c_str());
        saveImageAsPng(outFile.c_str(), image);
        
    }
}




/**
 * @param filename
 * @param feature
 */
void ImageExtractor::saveHogFeatureAsCsv(const char* filename, HOGFeature* feature)
{
    FILE* fp = fopen(filename, "w");
    
    fprintf(fp, "Image Width; Image Height; Block Width; Block Height; Cell Width; Cell Height;\n");
    fprintf(fp, "%d;%d;%d;%d;%d;%d;\n", feature->m_imageWidth, feature->m_imageHeight, 
            feature->m_blockWidth, feature->m_blockHeight,
            feature->m_cellWidth, feature->m_cellHeight);
    
    fprintf(fp, "color; block x; block y; cell x; celly; bin0; bin1; bin2; bin3; bin4; bin5; bin6; bin7; bin8;\n");
    
    for (int c=0; c < 3; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        unsigned int* pBin = feature->getBin(bx, by, x, y, c);

                        fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);

                        for (int i=0; i < 9; i++)
                            fprintf(fp, "%d; ", pBin[i]);

                        fprintf(fp, "\n");
                    }
    
    
    
    fclose(fp);
}

/**
 * Encode PNG 
 * http://www.labbookpages.co.uk/software/imgProc/libPNG.html
 * @param file
 * @param box
 */
void ImageExtractor::saveImageAsPng(const char* filename, Image* image)
{
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
        
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) 
    {
        printf("[ERROR] Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }
    
    // Initialize write structure
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) 
   {
      printf("[ERROR] Could not allocate write struct\n");
      code = 1;
      goto finalise;
   }

   // Initialize info structure
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      printf("[ERROR] Could not allocate info struct\n");
      code = 1;
      goto finalise;
   }
   
   // Setup Exception handling
   if (setjmp(png_jmpbuf(png_ptr))) {
      printf("[ERROR] Error during png creation\n");
      code = 1;
      goto finalise;
   }
   
   png_init_io(png_ptr, fp);

   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, image->m_width, image->m_height,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   // Set title
//   if (title != NULL) 
//   {
//      png_text title_text;
//      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
//      title_text.key = "Title";
//      title_text.text = title;
//      png_set_text(png_ptr, info_ptr, &title_text, 1);
//   }

   png_write_info(png_ptr, info_ptr);
   
   // Allocate memory for one row (3 bytes per pixel - RGB)
   row = new png_byte[3 * image->m_width];

   // Write image data
   int x, y;
   for (y=0 ; y < image->m_height ; y++) 
   {
      for (x=0 ; x < image->m_width ; x++) 
      {
          row[x*3] = image->getR(x, y);
          row[x*3+1] = image->getG(x, y);
          row[x*3+2] = image->getB(x, y);
      }
      png_write_row(png_ptr, row);
   }

   // End write
   png_write_end(png_ptr, NULL);
   
   finalise:
   if (fp != NULL) fclose(fp);
   if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   if (row != NULL) delete row;

   // return code;
}