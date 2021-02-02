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

#ifndef FPGAHOGPROCESSOR_H
#define FPGAHOGPROCESSOR_H

#include "HOGFeature.h"
#include "Image.h"

#include <vector>

/**
 * This class is the responsible for creating HOG features from images
 */
class FPGAHOGProcessor 
{
public:
    FPGAHOGProcessor();
    FPGAHOGProcessor(int cellWidth, int cellHeight, int blockWidth, int blockHeight);

    virtual ~FPGAHOGProcessor();
    
public:
    Image* createHOGVisualization(HOGFeature* feature);
    HOGFeature* createFeature(Image* image);

    std::vector<Image*> createHogImages(std::vector<Image*> images);
    std::vector<HOGFeature*> createHogFeatures(std::vector<Image*> images);

    HOGFeature* allocateMemFeature(Image* image);
    double* allocateMemGradient(Image* image);
    
    void computeGradient(Image* image, double* pGradientMag, double* pGradientOrient);
    void computeHistogram(Image* image, double* pGradientMag, double* pGradientOrient, HOGFeature* preNorm);
    void computeNorm(Image* image, HOGFeature* preNorm, HOGFeature* feature);

    //  __attribute__((deprecated));
protected:
    void computeGradient(Image* image, HOGFeature* feature, int xcell, int ycell, int colorChannel, double* magOut, double* orientationOut);
    void getCellX();
    void getCellY();
    void interpolateBins(double gMag, double gOri, int* bin0, int* bin1, double* mag0, double* mag1);
    double cellInterpolation(double centerx, double centery, double x, double y);
    double cellGaussian(double x, double y);
    
public:
    bool m_rotateHog;

public:
    int m_cellWidth;    // number of pixels per cell x
    int m_cellHeight;
    int m_blockWidth;   // number of cells per block in x
    int m_blockHeight;  // number of cells per block in y
    
};

#endif /* HOGPROCESSOR_H */

