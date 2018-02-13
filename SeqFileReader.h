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
 * File:   SeqFileReader.h
 * Author: dcr
 *
 * Created on February 1, 2018, 3:37 PM
 */

#ifndef SEQFILEREADER_H
#define SEQFILEREADER_H

#include "SeqFileHeader.h"
#include "Image.h"

#include <string>

/**
 * Seq File Reader
 * Some inspiration from
 * https://github.com/soft-matter/pims/blob/master/pims/norpix_reader.py
 * https://github.com/pdollar/toolbox/blob/master/videos/seqReaderPlugin.m
 */
class SeqFileReader {
public:
    SeqFileReader(const char* file);
    virtual ~SeqFileReader();
    
public:
    unsigned short readUSHORT();
    int readDWORD();
    int readLONG();
    double readDOUBLE();
    
    std::string readWideCharString(int len);
    void scanNextPngStartMarker();
    void scanNextJpegStartMarker();
    void readPngImageData(SeqFileHeader* header, Image* image);
    void readJpegImageData(SeqFileHeader* header, Image* image);
    void readRawImageData(Image* image);
    void readImageData(SeqFileHeader* header, Image* image);
    void skipImageData(SeqFileHeader* header);
    
    bool isOpen();
    void open();
    void close();
    void readHeader(SeqFileHeader* header);
    
private:
    FILE*       m_fp;
    std::string m_path;
};

#endif /* SEQFILEREADER_H */

