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
 * File:   Image.h
 * Author: dcr
 *
 * Created on February 10, 2018, 8:11 AM
 */

#ifndef IMAGE_H
#define IMAGE_H

class Image 
{
public:
    Image();
    virtual ~Image();
    
public:
    int get(int x, int y, int colorChannel);
    void set(int x, int y, int colorChannel, int v);
    int getR(int x, int y);
    int getG(int x, int y);
    int getB(int x, int y);
    int getBytes();
    void convertToYuv();
    void resizeFrom(Image* src);
    
private:
    int interpolate(double x, double y, int color);
    double interpolate_1D(double N0, double N1, double N2, double N3, double t) ;
    
public:
    int m_width;
    int m_height;
    int m_stride;   // number of pixels from a pixel in one row to the pixel to the
                    // next row
    unsigned char* m_buffer;
    bool           m_ownBuffer;
    
    int m_objId;    // context information
};

#endif /* IMAGE_H */

