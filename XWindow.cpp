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
 * File:   XWindow.cpp
 * Author: dcr
 * 
 * Created on February 6, 2018, 11:47 AM
 */

#include "XWindow.h"
#include "BoundingBox.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>

XWindow::XWindow()
{
    m_imageData = NULL;
}

XWindow::XWindow(const XWindow& orig)
{
}

XWindow::~XWindow()
{
    if (m_imageData != NULL)
        delete m_imageData;
}

void XWindow::flush()
{
    XFlush(m_display);
}

void XWindow::drawBoundingBox(BoundingBox& box)
{
    //        printf("drawing box %d - %d %d %d %d\n", i, (int) box.m_x, (int)box.m_y, (int)box.m_width, (int)box.m_height);
        
    if (box.m_occluded)
    {
        XSetForeground(m_display, m_gc, m_greyColor.pixel); 
    }
    else
    {
//            printf("%s\n", box.m_objLabel.c_str());

        if (box.isPerson())
            XSetForeground(m_display, m_gc, m_redColor.pixel); 
        else
            XSetForeground(m_display, m_gc, m_yellowColor.pixel); 
    }

    XDrawRectangle(m_display, m_window, m_gc,
            (int) box.m_x*m_zoom,
            (int) box.m_y*m_zoom, 
            (int)box.m_width*m_zoom,
            (int)box.m_height*m_zoom);
}

void XWindow::drawBoundingBoxes(std::vector<BoundingBox> boxes)
{
    for (int i=0; i < boxes.size(); i++)
    {
        BoundingBox box = boxes.at(i);
        
        drawBoundingBox(box);

    }
}

void XWindow::drawRGBImage(Image* image )
{
    XImage* bitmap;
    int width = image->m_width * m_zoom;
    int height = image->m_height * m_zoom;
    
    for (int y=0; y < height; y++)
        for (int x=0; x < width; x++)
        {
            // XWindows Format = BGRA 
            m_imageData[((y*width)+x)*4+0] = image->getB(x/m_zoom,y/m_zoom);
            m_imageData[((y*width)+x)*4+1] = image->getG(x/m_zoom,y/m_zoom);
            m_imageData[((y*width)+x)*4+2] = image->getR(x/m_zoom,y/m_zoom);
        }    
    
    bitmap = XCreateImage(m_display, DefaultVisual(m_display,DefaultScreen(m_display)),
            24, ZPixmap, 0, (char*) m_imageData, width, height,
            32, 0);
            
    if (bitmap == NULL)
    {
        printf("[ERROR] cannot create pixmap\n");
        exit(-1);
    }
     
    XPutImage(m_display, m_window, m_gc, bitmap, 0,0, 0,0 , width, height);
            
    XFree(bitmap);
}

void XWindow::create(int width, int height, int bitDepth, int zoom)
{
    m_display = XOpenDisplay(0);
    m_zoom = zoom;
        
    if (m_display==0)
    {
        printf("[ERROR] cannot open DISPLAY\n");
        exit(-1);
    }
    
    int screen = DefaultScreen(m_display);
    int depth  = DefaultDepth(m_display, screen);
    printf("Default depth: %d\n", depth);
        
    unsigned long background = 0x000000;     // WhitePixel(d, screen);
    unsigned long foreground = 0xFFFFFF;     // BlackPixel(d, screen);
    
    unsigned long white = WhitePixel(m_display, screen);
    unsigned long black = BlackPixel(m_display, screen);
    
//    m_window = XCreateWindow(m_display, DefaultRootWindow(m_display), 0, 0, width, height, 0,
//            bitDepth,    // depth
//            CopyFromParent,     // 
//            CopyFromParent,     // visual
//            0,                  // 
//            0);                 // attributes
    
    m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, width*zoom, height*zoom, 1, white, black );

    if (m_window==0)
    {
        printf("[ERROR] cannot create window\n");
        exit(-1);
    }
        
    m_gc = XCreateGC ( m_display, m_window, 0 , NULL );

    if (m_gc==0)
    {
        printf("[ERROR] cannot create GC\n");
        exit(-1);
    }

//        int screen = DefaultScreen(d);
//        
    
    
    printf("GC created\n");
    printf("background=%d foreground=%d\n", (int) background, (int) foreground);

    XMapWindow(m_display, m_window);
    
    XColor dummy;
     XAllocNamedColor(m_display, DefaultColormap(m_display, screen), "red", &m_redColor, &dummy);
     XAllocNamedColor(m_display, DefaultColormap(m_display, screen), "grey", &m_greyColor, &dummy);
     XAllocNamedColor(m_display, DefaultColormap(m_display, screen), "yellow", &m_yellowColor, &dummy);
     
     
     unsigned int line_width = 3;		/* line width for the GC.       */
    int line_style = LineSolid;		/* style for lines drawing and  */
    int cap_style = CapButt;		/* style of the line's edje and */
    int join_style = JoinBevel;		/*  joined lines.		*/
  
    XSetLineAttributes(m_display, m_gc, line_width, line_style, cap_style, join_style);
      
    m_imageData = new unsigned char[height*zoom*width*zoom*4];
}
    

