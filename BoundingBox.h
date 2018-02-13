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
 * File:   BoundingBox.h
 * Author: dcr
 *
 * Created on February 8, 2018, 9:10 AM
 */

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <string>

class BoundingBox {
public:
    BoundingBox(double x, double y, double w, double h, bool occluded);
    BoundingBox(const BoundingBox& orig);
    virtual ~BoundingBox();

public:
    bool isPerson();
    void centerBox(int nw, int nh);
    
public:
    double m_x;
    double m_y;
    double m_width;
    double m_height;
    bool m_occluded;
    int m_objId;
    std::string m_objLabel;
};

#endif /* BOUNDINGBOX_H */

