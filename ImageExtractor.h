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
 * File:   ImageExtractor.h
 * Author: dcr
 *
 * Created on February 9, 2018, 6:14 PM
 */

#ifndef IMAGEEXTRACTOR_H
#define IMAGEEXTRACTOR_H

#include "BoundingBox.h"
#include "Image.h"
#include "HOGFeature.h"

#include <vector>
#include <string>

class ImageExtractor {
public:
    ImageExtractor();
    ImageExtractor(const ImageExtractor& orig);
    virtual ~ImageExtractor();

public:
    void setExtractionPath(const char* path);
    void setFrameNumber(int n);
    void setImage(Image* image);
    void extractFrame();
    void savePersons(std::vector<Image*> persons);
    void saveHogPersons(std::vector<Image*> persons);
    void saveHogFeatures(std::vector<HOGFeature*> persons);
    void saveImageAsPng(const char* file, Image* image);
    void saveHogFeatureAsCsv(const char* filename, HOGFeature* feature);
    std::vector<Image*> getPersonsHigher(std::vector<BoundingBox> boxes, int minHeight);
    std::vector<Image*> resizePersons(std::vector<Image*> persons);
    
public:
    Image* m_image;
    int m_frameNumber;
    std::string m_extractionPath;
    
    bool m_resize;
    int m_resizeX;
    int m_resizeY;
};

#endif /* IMAGEEXTRACTOR_H */

