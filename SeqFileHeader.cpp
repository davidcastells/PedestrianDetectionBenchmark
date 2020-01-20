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
 * File:   SeqFileHeader.cpp
 * Author: dcr
 * 
 * Created on February 1, 2018, 5:50 PM
 */

#include "SeqFileHeader.h"

#include <stdio.h>
#include <stdarg.h>

#include <cstdarg>
#include <string>

using namespace std;
        
SeqFileHeader::SeqFileHeader()
{
}

SeqFileHeader::SeqFileHeader(const SeqFileHeader& orig)
{
}

SeqFileHeader::~SeqFileHeader()
{
}

std::string  SeqFileHeader::imagetFormatDescription()
{
    switch (imageFormat)
    {
        case 100:
        case 200:
            return "raw";
        case 101:
            return "brgb8";
        case 102:
        case 201:
            return "jpg";
        case 103:
            return "jbrgb";
        case 001:
        case 002:
            return "png";
        default:
            return "unknown";
    }
}
    

std::string SeqFileHeader::format(const char *fmt, ...)
{
    std::string result = "";

    char buffer[256];
    va_list args;
    va_start (args, fmt);
    vsprintf (buffer,fmt, args);
    
    result.append(buffer);
    
    va_end (args);
  
    return result;
}

std::string SeqFileHeader::toString()
{
    std::string ret = "";

    ret += format("Magic: 0x%08X\n", magic);
    ret += "Name: " + name + "\n";
    ret += format("Version: 0x%08X\n", version);
    ret += format("Header Size: 0x%08X\n", headerSize);
    ret += "Description: " + description + "\n";
    ret += format("width: %d\n" , width);
    ret += format("height: %d\n" , height);
    ret += format("bit depth: %d\n" , bitDepth);
    ret += format("bit depth real: %d\n" , bitDepthReal);
    ret += format("image size bytes: %d\n" , imageSizeBytes);
    ret += format("image format: %d = " , imageFormat);
    ret += imagetFormatDescription();
    ret += "\n";
    ret += format("allocated frames: %d\n" , allocatedFrames);
    ret += format("origin: %d\n" , origin);
    ret += format("true image size: %d\n" , trueImageSize);
    ret += format("suggested frame rate: %lf\n" , suggestedFrameRate);
    ret += format("description format: 0x%08X\n" , descriptionFormat);
    ret += format("reference frame: %d\n" , referenceFrame);
    ret += format("fixed size: %d\n" , fixedSize);
    ret += format("falgs: 0x%08X\n" , flags);
    ret += format("bayer pattern: 0x%08lX\n" , bayerPattern);
    ret += format("time offset: %ld us\n" , timeOffsetUS);
    ret += format("extended header size: %ld\n" , extendedHeaderSize);
    ret += format("compression format: %d\n" , compressionFormat);
    ret += format("reference time: %ld.%03d.%03d\n" ,referenceTimeS, referenceTimeMS, referenceTimeUS);
        
    return ret;
}
