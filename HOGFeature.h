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
 * File:   HOGFeature.h
 * Author: dcr
 *
 * Created on February 10, 2018, 11:22 AM
 */

#ifndef HOGFEATURE_H
#define HOGFEATURE_H

/**
 * This is a HOG feature that described an image
 * 
 * @param imageWidth
 * @param imageHeight
 * @param cellWidth
 * @param cellHeight
 * @param blockWidth
 * @param blockHeight
 * @param colorChannels
 */
class HOGFeature 
{
private:
    HOGFeature(){};
    
public:
    HOGFeature(int imageWidth, int imageHeight, int cellWidth, int cellHeight, int blockWidth, int blockHeight, int colorChannels);
    //HOGFeature(const HOGFeature& orig);
    virtual ~HOGFeature();

public:
    HOGFeature* createWindow(int blockx, int blocky,  int blockWidth, int blockHeight);
    double* getBin(int blockx, int blocky, int cellx, int celly, int color);
    int getImageXFromCellIndexX(int xblock, int xcell);
    int getImageYFromCellIndexY(int yblock, int xcell);
    int getCellsInAxisX();
    int getCellsInAxisY();
    int getOverlappingBlocksInAxisX();
    int getOverlappingBlocksInAxisY();
    int getBlocksInAxisX();
    int getBlocksInAxisY();
    int getTotalBins();
    
public:
    /** It contains 3 hog features (1 for each color channel) */
    double* m_buffer;
    
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;
//    unsigned int m_numCellsX;
//    unsigned int m_numCellsY;
    unsigned int m_numBlocksX;
    unsigned int m_numBlocksY;
    
    unsigned int m_colorChannels;
    
    unsigned int m_cellWidth;       // pixels per cell x
    unsigned int m_cellHeight;      // pixels per cell y
    unsigned int m_blockWidth;      // cells per block x
    unsigned int m_blockHeight;     // cells per block y
    
    int m_objId;
};

#endif /* HOGFEATURE_H */

