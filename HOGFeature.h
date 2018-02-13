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

class HOGFeature {
public:
    HOGFeature(int width, int height);
    //HOGFeature(const HOGFeature& orig);
    virtual ~HOGFeature();

public:
    unsigned int* getBin(int x, int y, int color);
    int getCellX(int xcell);
    int getCellY(int xcell);
    
    
public:
    /** It contains 3 hog features (1 for each color channel) */
    unsigned int* m_buffer;
    
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;
    unsigned int m_numCellsX;
    unsigned int m_numCellsY;
};

#endif /* HOGFEATURE_H */

