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


/**
 * 
 * @param imageWidth
 * @param imageHeight
 * @param cellWidth
 * @param cellHeight
 * @param blockWidth number of cells that form a block
 * @param blockHeight
 */
HOGFeature::HOGFeature(int imageWidth, int imageHeight, int cellWidth, int cellHeight, int blockWidth, int blockHeight, int colorChannels)
{
    assert(imageWidth % cellWidth == 0);
    assert(imageHeight % cellHeight == 0);
    assert(imageWidth % (cellWidth*blockWidth) == 0);
    assert(imageHeight % (cellHeight*blockHeight) == 0);

    
    m_imageWidth = imageWidth;
    m_imageHeight = imageHeight;
    
    m_cellWidth = cellWidth;
    m_cellHeight = cellHeight;
    m_blockWidth = blockWidth;
    m_blockHeight = blockHeight;
    
    m_colorChannels = colorChannels;
    
    int cellsInXAxis = imageWidth / cellWidth;      // how many cells fit in image width
    int cellsInYAxis = imageHeight / cellHeight;
    
    int blocksInXAxis = cellsInXAxis / blockWidth;  // how many blocks fit in image width
    int blocksInYAxis = cellsInYAxis / blockHeight; // how many blocks fit in image height
    
    m_numBlocksX = blocksInXAxis * 2 - 1;
    m_numBlocksY = blocksInYAxis * 2 - 1;
    
    assert(m_numBlocksX > 2);
    assert(m_numBlocksY > 2);
    
//    m_numCellsX = (imageWidth / cellWidth) * 2 -1;
//    m_numCellsY = (imageHeight / cellHeight) * 2 -1;
    
    int cellsPerRow = (m_numBlocksX * m_blockWidth);
    int cellsPerCol = (m_numBlocksY * m_blockHeight);
    
    m_buffer = new double[cellsPerRow*cellsPerCol*m_colorChannels*9];
    
    for (int i=0; i < cellsPerRow*cellsPerCol*m_colorChannels*9; i++)
        m_buffer[i] = 0;
    
}

HOGFeature* HOGFeature::createWindow(int blockxStart, int blockyStart, int blockWidth, int blockHeight)
{
    HOGFeature* ret = new HOGFeature();
    
//    assert(imageWidth % cellWidth == 0);
//    assert(imageHeight % cellHeight == 0);
//    assert(imageWidth % (cellWidth*blockWidth) == 0);
//    assert(imageHeight % (cellHeight*blockHeight) == 0);
    
    //printf("Creating sub-block from block (%d,%d) and size (%d,%d)\n", blockxStart, blockyStart, blockWidth, blockHeight);
    
    ret->m_imageWidth = (blockWidth +1)/2 * m_blockWidth * m_cellWidth;
    ret->m_imageHeight = (blockHeight+1)/2 * m_blockHeight * m_cellHeight;

    ret->m_cellWidth = m_cellWidth;
    ret->m_cellHeight = m_cellHeight;
    ret->m_blockWidth = m_blockWidth;
    ret->m_blockHeight = m_blockHeight;
    ret->m_colorChannels = m_colorChannels;
    
//    int cellsInXAxis = imageWidth / cellWidth;      // how many cells fit in image width
//    int cellsInYAxis = imageHeight / cellHeight;
//    
//    int blocksInXAxis = cellsInXAxis / blockWidth;  // how many blocks fit in image width
//    int blocksInYAxis = cellsInYAxis / blockHeight; // how many blocks fit in image height
    
    ret->m_numBlocksX = blockWidth;     // blocksInXAxis * 2 - 1;
    ret->m_numBlocksY = blockHeight;    // blocksInYAxis * 2 - 1;
    
//    m_numCellsX = (imageWidth / cellWidth) * 2 -1;
//    m_numCellsY = (imageHeight / cellHeight) * 2 -1;
    
    int cellsPerRow = (blockWidth * m_blockWidth);
    int cellsPerCol = (blockHeight * m_blockHeight);
    
    ret->m_buffer = new double[cellsPerRow*cellsPerCol*m_colorChannels*9];
    
    for (int c=0; c < m_colorChannels; c++)
        for (int by=0; by < blockHeight; by++ )
            for (int bx=0; bx < blockWidth; bx++ )
            {
                for (int cy = 0; cy < m_blockHeight; cy++)
                    for (int cx = 0; cx < m_blockWidth; cx++)
                    {
                        double* pSrc = getBin(blockxStart + bx, blockyStart+by, cx, cy, c);
                        double* pDst = ret->getBin(bx, by, cx, cy, c);
                        
                        for (int i=0; i < 9; i++)
                            pDst[i] = pSrc[i];                        
                    }
            }
    
    
    return ret;
}

int HOGFeature::getCellsInAxisX()
{
    return m_imageWidth / m_cellWidth;      // how many cells fit in image width
}



int HOGFeature::getCellsInAxisY()
{
    return m_imageHeight / m_cellHeight;
}

int HOGFeature::getOverlappingBlocksInAxisX()
{
    return m_numBlocksX;
}

int HOGFeature::getOverlappingBlocksInAxisY()
{
    return m_numBlocksY;
}

/**
 * 
 * @return 
 */
int HOGFeature::getBlocksInAxisX()
{
    return getCellsInAxisX() / m_blockWidth;  // how many blocks fit in image width
}

/**
 * 
 * @return the number of non-overlapping blocks in Y axis
 */
int HOGFeature::getBlocksInAxisY()
{
    return getCellsInAxisY() / m_blockHeight; // how many blocks fit in image height
}


int HOGFeature::getTotalBins()
{
    int totalBins = m_numBlocksX * m_numBlocksY * m_blockWidth * m_blockHeight * 9 * m_colorChannels;
    
    return totalBins;
}

/**
 * Return the starting x position to the first pixel of the cell x index
 * 
 * @param xcell
 * @return 
 */
int HOGFeature::getImageXFromCellIndexX(int xblock, int xcell)
{
//    int blx = getBlocksInAxisX();
//    int cellx = xcell;
//    
//    if (xblock >= blx)
//    {
//        xblock -= blx;
//        cellx++;
//    }
//    
//    cellx += xblock * m_blockWidth;
    
    // WARNING: this is assuming a block width of 2
    
    return (xcell + xblock) * m_cellWidth;
}

/**
 * Return the starting y address of a cell
 * @param ycell
 * @return 
 */
int HOGFeature::getImageYFromCellIndexY(int yblock, int ycell)
{
//    int bly = getBlocksInAxisY();
//    int celly = ycell;
//    
//    if (yblock >= bly)
//    {
//        yblock -= bly;
//        celly++;
//    }
//    
//    celly += yblock * m_blockHeight;
//        
    // WARNING: this is assuming a block width of 2
    
    return (ycell + yblock) * m_cellHeight;
}

double* HOGFeature::getBin(int blockx, int blocky, int cellx, int celly, int color)
{
    int indexX = (blockx * m_blockWidth) + cellx;   // number of cell x
    int indexY = (blocky * m_blockHeight) + celly;  // number of cell y
    int cellsPerRow = (m_numBlocksX * m_blockWidth);
    int cellsPerCol = (m_numBlocksY * m_blockHeight);
    
    int index = color * (cellsPerRow*cellsPerCol) + (indexY*cellsPerRow) + indexX;
    return &m_buffer[index*9];
}

//HOGFeature::HOGFeature(const HOGFeature& orig)
//{
//}

HOGFeature::~HOGFeature()
{
    delete [] m_buffer;
}

