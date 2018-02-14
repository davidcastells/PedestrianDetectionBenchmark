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
 * File:   MatfileReader.h
 * Author: dcr
 *
 * Created on February 6, 2018, 11:39 AM
 */

#ifndef MATFILEREADER_H
#define MATFILEREADER_H

#include "Matfile.h"
#include "BoundingBox.h"

#include <string>
#include <vector>

#define MATLAB_DATA_CLASS_CELL      1
#define MATLAB_DATA_CLASS_STRUCT    2
#define MATLAB_DATA_CLASS_OBJECT    3
#define MATLAB_DATA_CLASS_CHAR      4
#define MATLAB_DATA_CLASS_DOUBLE    6
#define MATLAB_DATA_CLASS_INT8      8
#define MATLAB_DATA_CLASS_UINT8     9


#define MATLAB_DATA_TYPE_INT8       1
#define MATLAB_DATA_TYPE_UINT8      2
#define MATLAB_DATA_TYPE_INT16      3
#define MATLAB_DATA_TYPE_UINT16     4
#define MATLAB_DATA_TYPE_INT32      5
#define MATLAB_DATA_TYPE_UINT32     6
#define MATLAB_DATA_TYPE_SINGLE     7
#define MATLAB_DATA_TYPE_DOUBLE     9
#define MATLAB_DATA_TYPE_MATRIX     14


/**
 * This class implements a simple Matlab File (Matfile) Parser.
 * Its aim is only to be able to decode Caltech data, so we are not expecting
 * to use it for other purposes, although I guess it could be useful in 
 * other applications
 * 
 * Info from
 * https://www.mathworks.com/help/pdf_doc/matlab/matfile_format.pdf
 */
class MatfileReader 
{
public:
    MatfileReader(const char* file);
    virtual ~MatfileReader();
public:
    bool isOpen();
    void open();
    void close();
    std::string readString(int len);
    std::string getObjectLabel(int id);

    void setVerbose(int v);
    void readVdd();
    
    int getAnnotatedFrames();
    std::vector<BoundingBox> getBoundingBoxes(int frame);
    
    void readHeader();
    MatlabDataElement* readMatlabDataElement();
    void readMatlabDataElementIntoMatrix(MatlabMatrix* mat);
    MatlabMatrix* readMatrix(unsigned int size);
    MatlabStructHeader* readStructHeader();
    MatlabStructRow* readStructRow(int numFields);
    MatlabInt8Value* readInt8Value();
    MatlabUint8Value* readUint8Value();
    MatlabUint16Value* readUint16Value();
    MatlabDoubleValue* readDoubleValue();
    MatlabCell* readCell(unsigned int size);

public:
    char readInt8();
    unsigned char readUint8();
    unsigned short readUint16();
    unsigned int readUint32();
    unsigned long long readUint64();
    double readDouble();
    const char* dataTypeAsString(int dataType);
    const char* dataClassAsString(int cls);
    void readTag(unsigned int* type, unsigned int* size);
    void readMatrixFlags(MatlabMatrix* mat);
    std::vector<int> readMatrixDimensions();
    std::string readArrayName();
    void alignTo64Bit();
    void dumpFileOffset();
    int getNumElements(unsigned int dataType, unsigned int dataSize);
    
private:
    FILE*       m_fp;
    std::string m_path;
    
    MatfileHeader m_header;
    
    int m_verbose;
    
    MatlabMatrix* m_annotations;
};

#endif /* MATFILEREADER_H */

