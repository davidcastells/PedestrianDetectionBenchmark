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
 * File:   XWindow.h
 * Author: dcr
 *
 * Created on February 6, 2018, 11:47 AM
 */

#ifndef XWINDOW_H
#define XWINDOW_H

#include "BoundingBox.h"
#include "Image.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <vector>

class XWindow {
public:
    XWindow();
    XWindow(const XWindow& orig);
    virtual ~XWindow();
    
public:
    void create(int w, int h, int d, int zoom);
    void drawRGBImage(Image* image);
    void drawBoundingBoxes(std::vector<BoundingBox> boxes);
    void flush();
    
private:
    unsigned char* m_imageData;
    Display* m_display;
    Window m_window;
    GC m_gc;
    XColor m_redColor;
    XColor m_greyColor;
    XColor m_yellowColor;
    int m_zoom;
};

#endif /* XWINDOW_H */

