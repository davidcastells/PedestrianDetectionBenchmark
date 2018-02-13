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
 * File:   HOGFeature.cpp
 * Author: dcr
 * 
 * Created on February 10, 2018, 11:22 AM
 */

#include "HOGFeature.h"

#include <assert.h>
#include <stdio.h>

HOGFeature::HOGFeature(int imageWidth, int imageHeight)
{
    int cellWidth = 8;
    int cellHeight = 8;
    
    assert(imageWidth % cellWidth == 0);
    assert(imageHeight % cellHeight == 0);
    
    m_imageWidth = imageWidth;
    m_imageHeight = imageHeight;
    m_numCellsX = (imageWidth / cellWidth) * 2 -1;
    m_numCellsY = (imageHeight / cellHeight) * 2 -1;
    
    m_buffer = new unsigned int[m_numCellsX*m_numCellsY*3*9];
    
    for (int i=0; i < m_numCellsX*m_numCellsY*3*9; i++)
        m_buffer[i] = 0;
    
}

/**
 * Return the starting address of the cell
 * @param xcell
 * @return 
 */
int HOGFeature::getCellX(int xcell)
{
    if ((xcell * 8) >= m_imageWidth)
        return (xcell * 8) - m_imageWidth + 4;
    else
        return (xcell * 8);
}

/**
 * Return the starting y address of a cell
 * @param ycell
 * @return 
 */
int HOGFeature::getCellY(int ycell)
{
    if ((ycell * 8) >= m_imageHeight)
        return (ycell * 8) - m_imageHeight + 4;
    else
        return (ycell * 8);
}

unsigned int* HOGFeature::getBin(int x, int y, int color)
{
    int index = color * (m_numCellsX*m_numCellsY) + (y*m_numCellsX) + x;
    return &m_buffer[index*9];
}

//HOGFeature::HOGFeature(const HOGFeature& orig)
//{
//}

HOGFeature::~HOGFeature()
{
    delete m_buffer;
}

