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
 * File:   BufferedImage.h
 * Author: dcr
 *
 * Created on February 10, 2018, 8:11 AM
 */

#ifndef BUFFEREDIMAGE_H
#define BUFFEREDIMAGE_H

#include "Image.h"

/**
 * 
 */
class BufferedImage : public Image
{
public:
    BufferedImage(int width, int height);
    BufferedImage(unsigned char* buffer, int width, int height);
    //BufferedImage(const BufferedImage& orig);
    virtual ~BufferedImage();
private:

};

#endif /* BUFFEREDIMAGE_H */

