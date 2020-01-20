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
 * File:   BufferedImage.cpp
 * Author: dcr
 * 
 * Created on February 10, 2018, 8:11 AM
 */

#include "BufferedImage.h"

BufferedImage::BufferedImage(int width, int height)
{   
    m_width = width;
    m_stride = width;
    m_height = height;
    m_channels = 3;
    int imageSize = height*width*3;
    m_buffer = new unsigned char[imageSize];    
    m_ownBuffer = true;
    
    for (int i=0; i < imageSize; i++)
        m_buffer[i] = 0;
}

BufferedImage::BufferedImage(int width, int height, int channels)
{   
    m_width = width;
    m_stride = width;
    m_height = height;
    m_channels = channels;
    int imageSize = height*width*3;
    m_buffer = new unsigned char[imageSize];    
    m_ownBuffer = true;
    
    for (int i=0; i < imageSize; i++)
        m_buffer[i] = 0;
}

BufferedImage::BufferedImage(unsigned char* buffer, int width, int height)
{
    m_buffer = buffer;
    m_ownBuffer = false;
    m_width = width;
    m_stride = width;
    m_height = height;
}

//BufferedImage::BufferedImage(const BufferedImage& orig)
//{
//}

BufferedImage::~BufferedImage()
{
    // buffer is destroyed in parent class
}

