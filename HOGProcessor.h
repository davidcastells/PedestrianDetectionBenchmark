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
 * File:   HOGProcessor.h
 * Author: dcr
 *
 * Created on February 10, 2018, 11:21 AM
 */

#ifndef HOGPROCESSOR_H
#define HOGPROCESSOR_H

#include "HOGFeature.h"
#include "Image.h"

#include <vector>

/**
 * This class is the responsible for creating HOG features from images
 */
class HOGProcessor 
{
public:
    HOGProcessor();
    HOGProcessor(int cellWidth, int cellHeight, int blockWidth, int blockHeight);

    virtual ~HOGProcessor();
    
public:
    HOGFeature* createFeature(Image* image);
    Image* createHOGVisualization(HOGFeature* feature);
    std::vector<Image*> createHogImages(std::vector<Image*> images);
    std::vector<HOGFeature*> createHogFeatures(std::vector<Image*> images);

protected:
    void computeGradient(Image* image, HOGFeature* feature, int xblock, int yblock, int xcell, int ycell, int colorChannel, short* gradientX, short* gradientY) __attribute__((deprecated));
    void computeGradient(Image* image, HOGFeature* feature, int xcell, int ycell, int colorChannel, double* magOut, double* orientationOut);
    void computeHistogram(short* gradientX, short* gradientY, unsigned int* pBin) __attribute__((deprecated));
    void computeHistogram(double* magIn, double* orientationIn, double* pBin);
    void getCellX();
    void getCellY();
    
public:
    bool m_rotateHog;

public:
    int m_cellWidth;    // number of pixels per cell x
    int m_cellHeight;
    int m_blockWidth;   // number of cells per block in x
    int m_blockHeight;  // number of cells per block in y
};

#endif /* HOGPROCESSOR_H */

