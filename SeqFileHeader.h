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
 * File:   SeqFileHeader.h
 * Author: dcr
 *
 * Created on February 1, 2018, 5:50 PM
 */

#ifndef SEQFILEHEADER_H
#define SEQFILEHEADER_H

#include <string>

class SeqFileHeader {
public:
    SeqFileHeader();
    SeqFileHeader(const SeqFileHeader& orig);
    virtual ~SeqFileHeader();

public:
    std::string imagetFormatDescription();
    
public:
    int magic;
    std::string name;
    long long version;
    long long headerSize;
    std::string description;
    int width;
    int height;
    int bitDepth;
    int bitDepthReal;
    int imageSizeBytes;
    int imageFormat;
    int allocatedFrames;
    int origin;
    int trueImageSize;
    double suggestedFrameRate;
    long long descriptionFormat;
    int referenceFrame;
    int fixedSize;
    int flags;
    long long bayerPattern;
    long long timeOffsetUS;
    long long extendedHeaderSize;
    int compressionFormat;
    long long referenceTimeS;
    unsigned short referenceTimeMS;
    unsigned short referenceTimeUS;
    
    /**
     * HEADER_FIELDS = [
    ('magic', DWORD),
    ('name', '24s'),
    ('version', LONG),
    ('header_size', LONG),
    ('description', '512s'),
    ('width', DWORD),
    ('height', DWORD),
    ('bit_depth', DWORD),
    ('bit_depth_real', DWORD),
    ('image_size_bytes', DWORD),
    ('image_format', DWORD),
    ('allocated_frames', DWORD),
    ('origin', DWORD),
    ('true_image_size', DWORD),
    ('suggested_frame_rate', DOUBLE),
    ('description_format', LONG),
    ('reference_frame', DWORD),
    ('fixed_size', DWORD),
    ('flags', DWORD),
    ('bayer_pattern', LONG),
    ('time_offset_us', LONG),
    ('extended_header_size', LONG),
    ('compression_format', DWORD),
    ('reference_time_s', LONG),
    ('reference_time_ms', USHORT),
    ('reference_time_us', USHORT)
    # More header values not implemented
]

     */
    
public:
    std::string toString();
};

#endif /* SEQFILEHEADER_H */

