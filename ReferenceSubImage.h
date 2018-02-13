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
 * File:   ReferenceSubImage.h
 * Author: dcr
 *
 * Created on February 10, 2018, 8:47 AM
 */

#ifndef REFERENCESUBIMAGE_H
#define REFERENCESUBIMAGE_H

#include "Image.h"
#include "BoundingBox.h"

class ReferenceSubImage : public Image
{
public:
    ReferenceSubImage(Image* image, int x, int y, int w, int h);
    ReferenceSubImage(Image* image, BoundingBox* box);
//    ReferenceSubImage(const ReferenceSubImage& orig);
    virtual ~ReferenceSubImage();

public:
    void drawAngleLine(double angle, int color, int colorChannel);
    
public:
    Image* m_reference;
    int m_referenceX;
    int m_referenceY;
};

#endif /* REFERENCESUBIMAGE_H */

