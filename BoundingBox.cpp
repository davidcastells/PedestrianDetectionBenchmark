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
 * File:   BoundingBox.cpp
 * Author: dcr
 * 
 * Created on February 8, 2018, 9:10 AM
 */

#include <vector>

#include "BoundingBox.h"

BoundingBox::BoundingBox(double x, double y, double w, double h, bool occluded)
{
    m_x = x;
    m_y = y;
    m_width = w;
    m_height = h;
    m_occluded = occluded;
}

BoundingBox::BoundingBox(const BoundingBox& orig)
{
    m_x = orig.m_x;
    m_y = orig.m_y;
    m_width = orig.m_width;
    m_height = orig.m_height;
    m_occluded = orig.m_occluded;
    m_objId = orig.m_objId;
    m_objLabel = orig.m_objLabel;
}

bool BoundingBox::isPerson()
{
    return (m_objLabel.compare(std::string("person"))==0);           
}

BoundingBox::~BoundingBox()
{
}

/**
 * Collision detection. Two boxes colide is their intersection is non empty set.
 *  
 * @param box
 * @return 
 */
bool BoundingBox::colide(BoundingBox& box)
{
    int xa1 = m_x;
    int xa2 = m_x + m_width;
    
    int xb1 = box.m_x;
    int xb2 = box.m_x + box.m_width;
    
    if (xa1  >  xb2)    
        // no collision possible (a is to the right of b)
        return false;
    
    if (xa2 < xb1)
        // no collision possible (a if to the left of b)
        return false;
    
    int ya1 = m_y;
    int ya2 = m_y + m_height;
    
    int yb1 = box.m_y;
    int yb2 = box.m_y + box.m_height;
    
    if (ya1 > ya2)
        // no collision possible (a is to below b)
        return false;
    
    if (ya2 < yb1)
        // no collision possible (a is on top of b)
        return false;
    
    return true;
}

bool BoundingBox::colide(std::vector<BoundingBox> boxes)
{
    for (int i=0; i< boxes.size(); i++)
        if (colide(boxes.at(i)))
            return true;
    
    return false;
}

void BoundingBox::centerBox(int nw, int nh)
{
    double aspectRatio = (double) nw / (double) nh;
    
    int expectedWidth = m_height * aspectRatio;
    int expectedMiddle = expectedWidth / 2;
    int currentMiddle = m_width / 2;
    
    int new_x = (m_x + currentMiddle) - expectedMiddle;
    
    m_x = new_x;
    m_width = expectedWidth;
}

