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
 * File:   ImageIO.cpp
 * Author: dcr
 * 
 * Created on February 21, 2018, 5:01 PM
 */

#include "ImageIO.h"

#include <png.h>
#include <stdlib.h>
#include <assert.h>

ImageIO::ImageIO()
{
}

ImageIO::ImageIO(const ImageIO& orig)
{
}

ImageIO::~ImageIO()
{
    
}


void ImageIO::scanNextPngStartMarker(FILE* m_fp)
{
    unsigned long offset = ftell(m_fp);

    bool found = false;
    char header[8];    // 8 is the maximum size that can be checked
    
    do
    {
        size_t read = fread(header, 1, 8, m_fp);
    
        if (png_sig_cmp((png_byte*)header, 0, 8))
        {
            found = false;
            fseek(m_fp, offset++, SEEK_SET);
        }
        else
            found = true;
        
    } while (!found);
    
    // rollback to previous position            
    // fseek(m_fp, offset, SEEK_SET);
}

/**
 * We always create a 3 channels image
 * @param filename
 * @param numChannels
 * @return 
 */
BufferedImage* ImageIO::loadImage(const char* filename, int numChannels)
{
    FILE* m_fp = fopen(filename, "rb");
    
    if (m_fp == NULL)
    {
        printf("Failed to read\n");
        return NULL;
    }
    
    scanNextPngStartMarker(m_fp);
    unsigned long offset = ftell(m_fp);
    
//    printf("Marker offset: %ld\n", offset);
    
    // initialize stuff 
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        printf("[ERROR] png_create_read_struct failed");
        exit(-1);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        printf("[ERROR] png_create_info_struct failed");
        exit(-1);
    }
    
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        printf("[ERROR] Error during init_io");
        exit(-1);
    }

    png_init_io(png_ptr, m_fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    
//    printf("Image Width=%d Height=%d\n", width, height);
    
    BufferedImage* image = new BufferedImage(width, height);
    
    //assert(width == image->m_width);
    //assert(height == image->m_height);
    
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    
    if (color_type == PNG_COLOR_TYPE_RGB)
        bit_depth *= 3;
    
//    printf("Color Type=%d\n", color_type);
//    printf("Bit Depth=%d\n", bit_depth);
    
    assert(bit_depth == numChannels * 8);

    int number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);


    // read file 
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        printf("[ERROR] Error during read_image");
        exit(-1);
    }
    
    
    png_bytep row_pointers[height]; //(png_bytep*) malloc(sizeof(png_bytep) * height);
    for (int y=0; y<height; y++)
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
    
    png_read_image(png_ptr, row_pointers);
    
    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
//            printf("filling %d, %d\n", x, y);
            for (int c=0; c < 3; c++)
            {
//                if (numChannels < 3)
//                    // put the same value
//                    image->m_buffer[(y*image->m_stride+x)*3+c] = row_pointers[y][x];
//                else
                    image->m_buffer[(y*image->m_stride+x)*3+c] = row_pointers[y][x*3+c];
            }
        }
        
        free(row_pointers[y]);
        row_pointers[y] = NULL;
    }
    // &image->m_buffer[y*image->m_stride*3]; 

//    printf("done!\n");
    
    
    png_read_end(png_ptr, info_ptr);
    
//    printf("read end\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//    printf("destroy struct\n");
    
    fclose(m_fp);
//    printf("close\n");
    
    image->m_channels = numChannels;
    
//    printf("set channels\n");
        
    return image;
}