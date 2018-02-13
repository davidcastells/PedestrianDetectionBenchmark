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

void BoundingBox::centerBox(int nw, int nh)
{
    double factor = (double) m_height / (double) nh;
    
    int expectedWidth = nw * factor;
    int expectedMiddle = expectedWidth / 2;
    int currentMiddle = m_width / 2;
    
    int new_x = (m_x + currentMiddle) - expectedMiddle;
    
    m_x = new_x;
    m_width = expectedWidth;
}

