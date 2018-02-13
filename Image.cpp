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
 * File:   Image.cpp
 * Author: dcr
 * 
 * Created on February 10, 2018, 8:11 AM
 */

#include "Image.h"

#include <stdlib.h>
#include <math.h>

Image::Image()
{
    m_ownBuffer = false;
    m_buffer = NULL;
}

Image::~Image()
{
    if (m_ownBuffer)
    {
        if (m_buffer != NULL)
            delete m_buffer;
    }
}

int Image::getBytes()
{
    return m_width * m_height * 3;
}

void Image::convertToYuv()
{
    for (int y=0; y < m_height; y++)
        for (int x=0; x < m_width; x++)
        {
            int r = m_buffer[(y*m_stride+x)*3];
            int g = m_buffer[(y*m_stride+x)*3+1];
            int b = m_buffer[(y*m_stride+x)*3+2];
            int cy = 0.299 * r + 0.587 * g + 0.114 * b;
            int cu = 0.492 * (b-cy);
            int cv = 0.877 * (r-cy);
            m_buffer[(y*m_stride+x)*3] = cy;
            m_buffer[(y*m_stride+x)*3+1] = cu; // cu; // u;
            m_buffer[(y*m_stride+x)*3+2] = cv; // cv; //v;
        }
}

int Image::get(int x, int y, int color)
{
    return m_buffer[(y*m_stride+x)*3+color];
}

void Image::set(int x, int y, int color, int v)
{
    m_buffer[(y*m_stride+x)*3+color] = v;
}

int Image::getR(int x, int y)
{
    return m_buffer[(y*m_stride+x)*3];
}

int Image::getG(int x, int y)
{
    return m_buffer[(y*m_stride+x)*3+1];
}

int Image::getB(int x, int y)
{
    return m_buffer[(y*m_stride+x)*3+2];
}

/**
 * Inspired from https://github.com/Tug/vision/blob/master/ij-plugins/Bicubic.java
 * @param src
 */
void Image::resizeFrom(Image* src)
{
    double factor = (double) src->m_height / (double) m_height;
    
    for (int c=0; c < 3; c++)
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
            {
                double sx = x * factor;
                double sy = y * factor;

                int v = src->interpolate(sx, sy, c);
                
                set(x, y, c, v);
            }
}

/**
 * 
 * @param x
 * @param y
 * @return 
 */
int Image::interpolate(double x, double y, int color) 
{

    int ix = (int)x;
    int iy = (int)y;
    double p = x - ix; // sub-pixel offset in the x axis
    double q = y - iy; // sub-pixel offset in the y axis

//    if ((ix < 2) || (iy < 2) || (ix >= m_width-3) || (iy >= m_height-3))
//        return 0; // if pixel or neighbors are out of the image

    // Pixel neighborhood N;
    int dimN = 4;
    // this are the 16 neighbouting values needed by bicubic interpolation
    int N[4][4];    
    
    //int offset = (iy - 1) * width + (ix - 1); // offset to the top-left neighbor

    /**** 1. FILL IN THE NEIGHBOR SET "N[0] to N[15]" HERE ****/
    for(int i=-1; i<3; i++)
    {
        for(int j=-1; j<3; j++)
        {
            // x,y
            N[i][j] = get(ix+i, iy+j, color); //  pixels[(int)(offset + j*p + i*q*width)];
        }
    }

    int result = 0;
    
    // interpolate in the y direction using the neighbor set N
    /**** 2. COMPUTE THE INTERPOLATION RESULTS IN THE X-DIRECTION HERE ****/
    // interpolate each line
    double L[4];
    for(int j=0; j<4; j++)
    {
        L[j] = interpolate_1D(N[j][0], N[j][1], N[j][2], N[j][3], q) ;
    }
    // interpolate the y-axis results in the y direction and add to "result"
    double interp_res = interpolate_1D(L[0], L[1], L[2], L[3], p) ;
    /**** 3. FILL IN THE INTERPOLATION RESULT IN THE Y-DIRECTION****/
    result = interp_res;

    return result;
}

/**
 * Having 4 values corresponding to 4 (equispaced) points we find the value
 * from a new point t
 * 
 * The indexes of the points are -1, 0, 1, 2
 * 
 * @param N0
 * @param N1
 * @param N2
 * @param N3
 * @param t
 * @return 
 */
double Image::interpolate_1D(double N0, double N1, double N2, double N3, double t) 
{
    // cubic interpolation using four pixels

    /**** 4. COMPUTE THE INTERPOLATION COEFFICIENTS HERE ****/
    double c0 = 2*N1;
    double c1 = N2-N0;
    double c2 = 2*N0-5*N1+4*N2-N3;
    double c3 = -N0+3*N1-3*N2+N3;
    /**** 5. FILL IN THE CUBIC RESULT HERE ****/
    double res = (c3*t*t*t + c2*t*t + c1*t + c0)/2;
    res = round(res); 

    // Check for oveflow and underflow in computations
    if(res < 0) res = 0;
    if(res > 255) res = 255;

    return res;
}