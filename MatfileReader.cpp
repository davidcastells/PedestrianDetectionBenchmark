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
 * File:   MatfileReader.cpp
 * Author: dcr
 * 
 * Created on February 6, 2018, 11:39 AM
 */

#include "MatfileReader.h"

#include <stdio.h>
#include <stdlib.h>

#include <typeinfo>

MatfileReader::MatfileReader(const char* file)
{
    m_path = file;
    m_fp = NULL;
    m_verbose = 1;
    m_annotations = NULL;
}


MatfileReader::~MatfileReader()
{
    if (m_annotations != NULL)
        delete m_annotations;
    
    if (isOpen())
        close();
}

void MatfileReader::open()
{
    m_fp = fopen(m_path.c_str(), "rb");
}

void MatfileReader::close()
{
    fclose(m_fp);
}

bool MatfileReader::isOpen()
{
    return m_fp != NULL;
}

std::string MatfileReader::readString(int len)
{
    char s[len];
    
    fread(s, sizeof(char) , len, m_fp);
    
    std::string ret = "";
    
    for (int i=0; (i < len) && (s[i] != 0); i++)
        ret.append(1, (char) s[i]);
    
    return ret;
}

void MatfileReader::setVerbose(int v)
{
    m_verbose = v;
}

void MatfileReader::readVdd()
{
    if (!isOpen())
        open();
 
    if (m_verbose > 0)
        printf("Matfile\n");
    
    readHeader();

    m_annotations = (MatlabMatrix*) readMatlabDataElement();

}

/**
 * Return the number of annotated frames
 * @return 
 */
int MatfileReader::getAnnotatedFrames()
{
//    printf("m  = %s (%dx%d)\n", m->m_name.c_str(), m->m_dimensions.at(0), m->m_dimensions.at(1));
//
//    for (int i=0; i < m->m_structHeader->m_fieldNames.size(); i++)
//        printf("%d - %s\n",i, m->m_structHeader->m_fieldNames.at(i).c_str());
    
    //MatlabDataElement* v = m_annotations->get(0, 0);
    
//    printf("v type: %s\n", v->getType().c_str());
    
    MatlabMatrix* m = (MatlabMatrix*) m_annotations->get("nFrame");
    MatlabUint16Value* v2 = (MatlabUint16Value*) m->get(0, 0);
    
    //printf("v type: %s\n", v->getType().c_str());
    return  v2->m_value;
}

std::string MatfileReader::getObjectLabel(int id)
{
    MatlabMatrix* label = (MatlabMatrix*) m_annotations->get("objLbl");
    MatlabMatrix* record = (MatlabMatrix*) label->get(id-1);
    std::string ret = "";
    
    for (int i=0; i < record->totalSize(); i++)
        ret += (char) ((MatlabValue*)record->get(i))->toInt();
        
    return ret;
}

/**
 * Obtain the bounding boxes from the annotation list
 * @param frame
 * @return 
 */
std::vector<BoundingBox> MatfileReader::getBoundingBoxes(int frame)
{
    std::vector<BoundingBox> ret;
    
    MatlabMatrix* m = (MatlabMatrix*) m_annotations->get("objLists");
    
    MatlabMatrix* frameInfo = (MatlabMatrix*) m->get(frame);
    
//    printf("Frame Info: ");
//    frameInfo->dumpInfo();

    int numRows = frameInfo->totalSize();
    
    for (int i=0; i < numRows; i++)
    {
        MatlabMatrix* v2 = (MatlabMatrix*) frameInfo->get("id", i);

        int ObjId = ((MatlabValue*)v2->get(0))->toInt();

        std::string name = getObjectLabel(ObjId);
        
        
        // This is the object identifier

        v2 = (MatlabMatrix*) frameInfo->get("pos", i);
//        v2 = (MatlabMatrix*) frameInfo->get("posv", i);

//        v2->dumpInfo();
        
        int x = ((MatlabValue*)v2->get(0))->toInt();
        int y = ((MatlabValue*)v2->get(1))->toInt();
        int w = ((MatlabValue*)v2->get(2))->toInt();
        int h = ((MatlabValue*)v2->get(3))->toInt();
        
        v2 = (MatlabMatrix*) frameInfo->get("occl", i);
        
        bool occluded = ((MatlabValue*)v2->get(0))->toInt();

        BoundingBox rec(x, y, w, h, occluded);
        rec.m_objId = ObjId;
        rec.m_objLabel.assign(name);

//        printf("Label: %s\n", name.c_str());
//        printf("obj label: %s\n", rec.m_objLabel.c_str());
        
        ret.push_back(rec);
    }
    
    return ret;
}


void MatfileReader::readHeader()
{
    m_header.description = readString(116);
    m_header.dataOffset = readUint64();
    
    if (m_header.dataOffset == 0x2020202020202020)
        m_header.dataOffset = 0;
    
    m_header.version = readUint16();
    m_header.endianess = readUint16();
    
    if (m_verbose > 0)
    {
        printf("%s\n", m_header.toString().c_str());
    }
}

/**
 * 
 * @param dataType
 * @param dataSize in bytes
 * @return 
 */
int MatfileReader::getNumElements(unsigned int dataType, unsigned int dataSize)
{
    if (dataType == MATLAB_DATA_TYPE_MATRIX)
        return 1;
    else if (dataType == MATLAB_DATA_TYPE_UINT16)
        return dataSize/2;
    else if (dataType == MATLAB_DATA_TYPE_UINT8)
        return dataSize;
    else if (dataType == MATLAB_DATA_TYPE_DOUBLE)
        return dataSize/8;
    else
    {
        printf("[ERROR] unsupported type %s\n", dataTypeAsString(dataType));
        exit(-1);
    }
}

void MatfileReader::readMatlabDataElementIntoMatrix(MatlabMatrix* mat)
{
    int elementsStoredInMatrix = 0;
    
    do
    {
        unsigned int dataType;
        unsigned int dataSize;

        readTag(&dataType, &dataSize);

        if (m_verbose > 1)
        {
            printf("Data Type: %s\n", dataTypeAsString(dataType));
            printf("Data Size: %d\n", dataSize);
        }

        int numElements = getNumElements(dataType, dataSize);

        for (int i=0; i< numElements; i++)
        {
            if (m_verbose > 0)
                printf("Filling matrix pos[%d] / %d\n", elementsStoredInMatrix+i, mat->totalSize());

            if (dataType == MATLAB_DATA_TYPE_MATRIX)
                mat->set(elementsStoredInMatrix+i, readMatrix(dataSize));
            else if (dataType == MATLAB_DATA_TYPE_UINT16)
                mat->set(elementsStoredInMatrix+i, readUint16Value());
            else if (dataType == MATLAB_DATA_TYPE_UINT8)
                mat->set(elementsStoredInMatrix+i, readUint8Value());
            else if (dataType == MATLAB_DATA_TYPE_DOUBLE)
                mat->set(elementsStoredInMatrix+i, readDoubleValue());
            else
            {
                printf("[ERROR] unsupported type %s\n", dataTypeAsString(dataType));
                exit(-1);
            }
        }

        elementsStoredInMatrix += numElements;

        alignTo64Bit();
        
    } while (elementsStoredInMatrix < mat->totalSize());
}

MatlabDataElement* MatfileReader::readMatlabDataElement()
{
    unsigned int dataType;
    unsigned int dataSize;

    readTag(&dataType, &dataSize);
    
    if (m_verbose > 1)
    {
        printf("Data Type: %s\n", dataTypeAsString(dataType));
        printf("Data Size: %d\n", dataSize);
    }
    
    if (dataType == MATLAB_DATA_TYPE_MATRIX)
        return readMatrix(dataSize);
    else if (dataType == MATLAB_DATA_TYPE_UINT16)
        return readUint16Value();
    else if (dataType == MATLAB_DATA_TYPE_INT8)
        return readInt8Value();
    else if (dataType == MATLAB_DATA_TYPE_UINT8)
        return readUint8Value();
    else if (dataType == MATLAB_DATA_TYPE_DOUBLE)
        return readDoubleValue();
    else
    {
        printf("[ERROR] unsupported type %s\n", dataTypeAsString(dataType));
        exit(-1);
    }
    
    alignTo64Bit();
}

MatlabUint16Value* MatfileReader::readUint16Value()
{
    return new MatlabUint16Value(readUint16());
}

MatlabInt8Value* MatfileReader::readInt8Value()
{
    return new MatlabInt8Value(readInt8());
}

MatlabUint8Value* MatfileReader::readUint8Value()
{
    return new MatlabUint8Value(readUint8());
}

MatlabDoubleValue* MatfileReader::readDoubleValue()
{
    return new MatlabDoubleValue(readDouble());
}

/**
 * Read the rest of a matrix, once we have read the header tag identifying 
 * there is a matrix
 * @param size
 * @return 
 */
MatlabMatrix* MatfileReader::readMatrix(unsigned int size)
{
    MatlabMatrix* matrix = new MatlabMatrix();
    
    matrix->m_size = size;
    
    readMatrixFlags(matrix);
    matrix->setDimensions(readMatrixDimensions());
    matrix->m_name = readArrayName();

    if (m_verbose > 0)
        printf("Matrix \"%s\" (%dx%d) %s - size: 0x%08X\n", matrix->m_name.c_str(), 
                matrix->m_dimensions[0], matrix->m_dimensions[1],
                dataClassAsString(matrix->m_class), matrix->m_size);
    
    
            if (matrix->m_class == MATLAB_DATA_CLASS_STRUCT)
            {
                matrix->setStructHeader(readStructHeader());
                
                for (int i=0; i < matrix->totalSize(); i++)
                {
                    if (m_verbose > 0)
                        printf("Struct Row [%d] / %d\n", i, matrix->totalSize());
                    
                    matrix->set(i, readStructRow(matrix->m_structHeader->m_fieldNames.size()));
                }
            }
            else if (matrix->m_class == MATLAB_DATA_CLASS_DOUBLE)
            {
                //MatlabDoubleValue* v = readDoubleValue();
                readMatlabDataElementIntoMatrix(matrix);
                //printf("Value= %lf\n", v->m_value);

                alignTo64Bit();
            }
            else if (matrix->m_class == MATLAB_DATA_CLASS_CELL)
            {
                readMatlabDataElementIntoMatrix(matrix);
            }
            else if (matrix->m_class == MATLAB_DATA_CLASS_UINT8)
            {
                readMatlabDataElementIntoMatrix(matrix);
            }
            else if (matrix->m_class == MATLAB_DATA_CLASS_CHAR)
            {
                readMatlabDataElementIntoMatrix(matrix);
            }
            else
            {
                printf("[ERROR] unsupported class");
                exit(-1);
            }
        
    return matrix;
}

MatlabStructHeader* MatfileReader::readStructHeader()
{
    MatlabStructHeader* st = new MatlabStructHeader();

    unsigned int dataType;
    unsigned int dataSize;

    // @todo substitue by using readDataElement
    // MatlabUint32Value
    readTag(&dataType, &dataSize);
    
    if (m_verbose > 1)
    {
        printf("Field Name Length Type: %s\n", dataTypeAsString(dataType));
        printf("Field Name Length Size: %d\n", dataSize);
    }
    
    int fieldNameLength = readUint32();
    
    if (m_verbose > 1)
        printf("Field Name Length: %d\n", fieldNameLength);
    
    readTag(&dataType, &dataSize);
    
    if (m_verbose > 1)
    {
        printf("Field Names Data Type: %s\n", dataTypeAsString(dataType));
        printf("Field Names Data Size: %d\n", dataSize);
    }
    
    int numFields = dataSize / fieldNameLength;
    
    for (int i=0; i < numFields; i++)
    {
        std::string fieldName = readString(fieldNameLength);
        
        if (m_verbose > 0)
            printf("Field %d = %s\n", i, fieldName.c_str());
        
        st->m_fieldNames.push_back(fieldName);
    }
    
    alignTo64Bit();
    
    return st;
}

MatlabStructRow* MatfileReader::readStructRow(int numFields)
{
    MatlabStructRow* st = new MatlabStructRow();

    for (int i=0; i < numFields; i++)
    {
        st->m_fieldValues.push_back(readMatlabDataElement());
    }
    return st;
}





void MatfileReader::alignTo64Bit()
{
    unsigned long long offset = ftell(m_fp);
    
    while (offset % 8)
    {
        readUint8();
        offset = ftell(m_fp);
    }
        
}

void MatfileReader::dumpFileOffset()
{
    unsigned long int offset = ftell(m_fp);
    printf("File Offset: %08lX\n", offset);
}

std::string MatfileReader::readArrayName()
{
    unsigned int dataType;
    unsigned int dataSize;
    
    readTag(&dataType, &dataSize);
    

    if (m_verbose > 1)
    {
        printf("Array Name\n");
        printf("Sub Data Type: %s\n", dataTypeAsString(dataType));
        printf("Sub Data Size: %d\n", dataSize);
    }
    
    std::string name = "";
            
    if (dataSize)
    {
        name = readString(dataSize);
        if (m_verbose > 0)
            printf("Array Name: %s\n", name.c_str());
    }
    else
        if (m_verbose > 0)
            printf("No Array Name\n");
    
    alignTo64Bit();    

    return name;
}

std::vector<int> MatfileReader::readMatrixDimensions()
{
    std::vector<int> ret;
    
    unsigned int dataType;
    unsigned int dataSize;
    
    readTag(&dataType, &dataSize);
    
    if (m_verbose > 1)
    {
        printf("Matrix Dimensions\n");
        printf("Sub Data Type: %s\n", dataTypeAsString(dataType));
        printf("Sub Data Size: %d\n", dataSize);
    }
    
    if (m_verbose > 0)
        dumpFileOffset();
    
    for (int i=0; i < dataSize/4; i++)
    {
        int dim = readUint32();
        if (m_verbose > 1)
           printf("Dimesion %d = %d\n", i, dim);
        ret.push_back(dim);
    }
    
    return ret;
}

/**
 * 
 * @return the class
 */
void MatfileReader::readMatrixFlags(MatlabMatrix* mat)
{
    unsigned int dataType;
    unsigned int dataSize;
    
    readTag(&dataType, &dataSize);
    
    if (m_verbose > 1)
        printf("Matrix Flags: (%s, %d)\n", dataTypeAsString(dataType), dataSize);
    
    unsigned int arrayFlags = readUint32();
    int arrayFlags1 = arrayFlags >> (8*2) & 0xFFFF;
//    printf("Array Flags 1 (undef): %d\n", arrayFlags1);
    int arrayFlags2 = arrayFlags >> (8) & 0xFF;
//    printf("Array Flags 2 (flags): %d\n", arrayFlags2);
    mat->m_class = arrayFlags & 0xFF;
//    printf("Array Flags 3 (class): %d\n", arrayFlags3);
    int arrayFlags4 = readUint32();
//    printf("Array Flags 4 (undex): %d\n", arrayFlags4);
    
    if (m_verbose > 1)
        printf("Class: %s\n", dataClassAsString(mat->m_class));
}


const char* MatfileReader::dataClassAsString(int cls)
{
    switch(cls)
    {
        case MATLAB_DATA_CLASS_CELL: return "mxCELL_CLASS";
        case MATLAB_DATA_CLASS_STRUCT: return "mxSTRUCT_CLASS";
        case MATLAB_DATA_CLASS_DOUBLE: return "mxDOUBLE_CLASS";
        case MATLAB_DATA_CLASS_INT8: return "mxINT8_CLASS";
        case MATLAB_DATA_CLASS_UINT8: return "mxUINT8_CLASS";
        case MATLAB_DATA_CLASS_CHAR: return "mxCHAR_CLASS";
        default: return "mx unknown";
    }
}




const char* MatfileReader::dataTypeAsString(int dataType)
{
    switch (dataType)
    {
        case MATLAB_DATA_TYPE_INT8: return "miINT8";
        case MATLAB_DATA_TYPE_UINT8: return "miUINT8";
        case MATLAB_DATA_TYPE_INT16: return "miINT16";
        case MATLAB_DATA_TYPE_UINT16: return "miUINT16";
        case MATLAB_DATA_TYPE_INT32: return "miINT32";
        case MATLAB_DATA_TYPE_UINT32: return "miUINT32";
        case MATLAB_DATA_TYPE_SINGLE: return "miSINGLE";
        case MATLAB_DATA_TYPE_DOUBLE: return "miDOUBLE";
        case 12: return "miINT64";
        case 13: return "miUINT64";
        case MATLAB_DATA_TYPE_MATRIX: return "miMATRIX";
        case 15: return "miCOMPRESSED";
        case 16: return "miUTF8";
        case 17: return "miUTF16";
        case 18: return "miUTF32";
        default: return "Reserved";
    }
}

char MatfileReader::readInt8()
{
    char v = 0;
    fread(&v, 1 , 1, m_fp);
    return v;
}

unsigned char MatfileReader::readUint8()
{
    unsigned char v = 0;
    fread(&v, 1 , 1, m_fp);
    return v;
}

unsigned short MatfileReader::readUint16()
{
    unsigned short v = 0;
    fread(&v, 2 , 1, m_fp);
    return v;
}


unsigned int MatfileReader::readUint32()
{
    unsigned int v = 0;
    fread(&v, 4 , 1, m_fp);
    return v;
}

unsigned long long MatfileReader::readUint64()
{
    unsigned long long v = 0;
    fread(&v, 8 , 1, m_fp);
    return v;
}

double MatfileReader::readDouble()
{
    double v = 0;
    fread(&v, sizeof(double), 1, m_fp);
    return v;
}

/**
 * @deprecated
 * @param size
 * @return 
 */
MatlabCell* MatfileReader::readCell(unsigned int size)
{
    MatlabCell* cell = new MatlabCell();
    
    cell->m_matrix = readMatrix(size);
    
    return cell;
}

void MatfileReader::readTag(unsigned int* type, unsigned int* size)
{
    unsigned int v = readUint32();
    
    if (v & 0xFFFF0000)
    {
        // Small data
        *type = v & 0xFFFF;
        *size = (v>>16) & 0xFFFF;
    }
    else
    {
        *type = v;
        *size = readUint32();
    }
}