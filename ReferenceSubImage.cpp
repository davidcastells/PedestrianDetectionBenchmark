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
 * File:   ReferenceSubImage.cpp
 * Author: dcr
 * 
 * Created on February 10, 2018, 8:47 AM
 */

#include "ReferenceSubImage.h"
#include "Image.h"
#include "BoundingBox.h"

#include <math.h>

ReferenceSubImage::ReferenceSubImage(Image* image, int x, int y, int w, int h)
{
    m_width = w;
    m_height = h;
    m_stride = image->m_stride;
    m_buffer = &image->m_buffer[(int)(((y*image->m_stride)+x)*3)];
    m_ownBuffer = false;
    m_objId = 0;
    m_channels = image->m_channels;
    
    m_reference = image;
    m_referenceX = x;
    m_referenceY = y;
}


/**
 * Create a subimage with a reference to an original image
 * @param image
 * @param box
 */
ReferenceSubImage::ReferenceSubImage(Image* image, BoundingBox* box)
{
    m_reference = image;
    m_referenceX = box->m_x;
    m_referenceY = box->m_y;
    
    m_width = box->m_width;
    m_stride = image->m_stride;
    m_height = box->m_height;
    m_buffer = &image->m_buffer[(int)(((m_referenceY*m_stride)+m_referenceX)*3)];
    m_ownBuffer = false;
    m_objId = box->m_objId;
    m_channels = image->m_channels;
    
}

//ReferenceSubImage::ReferenceSubImage(const ReferenceSubImage& orig)
//{
//}

ReferenceSubImage::~ReferenceSubImage()
{
}


/**
 * Dirty algorithm for line drawing
 * @todo improve it
 * @param angle
 * @param color
 * @param colorChannel
 */
void ReferenceSubImage::drawAngleLine(double angle, int color, int colorChannel)
{
    int centerx = m_width / 2;
    int centery = m_height / 2;
    
    int v = get(centerx, centery, colorChannel);
    if (color > v)
        set(centerx, centery, colorChannel, color);
   
    int i = 0;
    int x = centerx;
    int y = centery;
    
    do
    {
        x = centerx + i * cos(angle);
        y = centery + i * sin(angle);
        
        i++;
        
        if (x < 8 && x>=0 && y >= 0 && y < 8)
        {
            int v = get(x, y, colorChannel);
            if (color > v)
                set(x, y, colorChannel, color);
        }
    } while (x >= 0 && x < 8 && y >= 0 && y < 8);
    
    i=0;
    do
    {
        x = centerx - i * cos(angle);
        y = centery - i * sin(angle);
        
        i++;
        
        if (x < 8 && x>=0 && y >= 0 && y < 8)
        {
            int v = get(x, y, colorChannel);
            if (color > v)
                set(x, y, colorChannel, color);
        }
    } while (x >= 0 && x < 8 && y >= 0 && y < 8);
}
