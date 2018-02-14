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

class HOGProcessor {
public:
    HOGProcessor();
    HOGProcessor(const HOGProcessor& orig);
    virtual ~HOGProcessor();
    
public:
    HOGFeature* createFeature(Image* image);
    Image* createHOGVisualization(HOGFeature* feature);
    std::vector<Image*> createHogImages(std::vector<Image*> images);
    std::vector<HOGFeature*> createHogFeatures(std::vector<Image*> images);

protected:
    void computeGradient(Image* image, HOGFeature* feature, int xcell, int ycell, int colorChannel);
    void computeHistogram(unsigned int* pBin);
    void getCellX();
    void getCellY();
    
public:
    bool m_rotateHog;
private:
    // gradients are short (not char) because we need 10 bits 
    short m_gradientx[8*8];
    short m_gradienty[8*8];
    
};

#endif /* HOGPROCESSOR_H */

