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
 * File:   Matfile.cpp
 * Author: dcr
 * 
 * Created on February 6, 2018, 4:41 PM
 */

#include "Matfile.h"

#include <stdio.h>
#include <stdlib.h>

std::string MatfileHeader::toString()
{
    std::string str = "";
    
    str.append("Description: ");
    str.append(description);
    str.append("\n");
//    if (dataOffset != 0)
//        printf("Subsys data offset: 0x%016lX", dataOffset);
//    else
//        printf("No Subsys\n");
//    
//    printf("Version: %d\n", version);
//    printf("Endianness: \"%c%c\"\n", (char)(endianess>>8), (char)(endianess&0xff));
    
    return str;
}

void MatlabValue::dumpInfo()
{
    printf("%s value = %d\n", getType().c_str(), toInt());
}

std::string MatlabStructHeader::getType()
{
    return "Struct Header";
}

void MatlabStructHeader::dumpInfo()
{
    for (int i=0; i < m_fieldNames.size(); i++)
       printf("%d - %s\n",i, m_fieldNames.at(i).c_str());
}


std::string MatlabStructRow::getType()
{
    return "Struct Row";
}


void MatlabStructRow::dumpInfo()
{
    printf("Row\n");
}

std::string MatlabCell::getType()
{
    return "Cell";
}


void MatlabCell::dumpInfo()
{
    printf("Cell\n");
}

MatlabMatrix::MatlabMatrix()
{
    m_structHeader = NULL;
}

MatlabMatrix::~MatlabMatrix()
{
    m_values.clear();
    m_dimensions.clear();
    
    if (m_structHeader != NULL)
        delete m_structHeader;
}

void MatlabMatrix::dumpInfo()
{
    printf("%s ", getType().c_str());
    printf("name  = \"%s\" (%dx%d)\n", m_name.c_str(), m_dimensions.at(0), m_dimensions.at(1));
    
    if (m_structHeader != NULL)
        m_structHeader->dumpInfo();
}


void MatlabMatrix::setStructHeader(MatlabStructHeader* header)
{
    m_structHeader = header;
}

int MatlabMatrix::totalSize()
{
    int totalSize = 1;
    
    for (int i=0; i < m_dimensions.size(); i++)
    {
        totalSize *= m_dimensions.at(i);
    }    
    
    return totalSize;
}

void MatlabMatrix::setDimensions(std::vector<int> dimensions)
{
    m_dimensions = dimensions;
    
    int total = totalSize();
    
    m_values.resize(total);
}

void MatlabMatrix::set(int i, MatlabDataElement* data)
{
    m_values[i] = data;
}   


void MatlabMatrix::set(int x, int y, MatlabDataElement* data)
{
    int width = m_dimensions.at(0);
    int index = y * width + x;
    
    set(index, data);
}   

std::string MatlabMatrix::getType()
{
    if (m_structHeader == NULL)
        return "Matrix";
    else
        return "Matrix (Struct)";
}

int MatlabMatrix::getFieldPos(std::string fieldName)
{
    if (m_structHeader == NULL)
        return -1;
    
    for (int i=0; i < m_structHeader->m_fieldNames.size(); i++)
        if (fieldName.compare(m_structHeader->m_fieldNames.at(i)) == 0)
            return i;
    
    return -1;
}

MatlabDataElement* MatlabMatrix::get(std::string fieldName)
{
    return get(fieldName, 0);
}

MatlabDataElement* MatlabMatrix::get(std::string fieldName, int rowNum)
{
    int pos = getFieldPos(fieldName);
    
    if (pos == -1)
        return NULL;
    
    MatlabStructRow* row = (MatlabStructRow*) m_values[rowNum];
    
    return row->m_fieldValues.at(pos);
}

MatlabDataElement* MatlabMatrix::get(int index)
{
    return m_values[index];
}

MatlabDataElement* MatlabMatrix::get(int x, int y)
{
    int width = m_dimensions.at(0);
    int index = y * width + x;
    
    return get(index);
}   

MatlabInt8Value::MatlabInt8Value(char v)
{
    m_value = v;
}

std::string MatlabInt8Value::getType()
{
    return "Int8";
}

int MatlabInt8Value::toInt()
{
    return m_value;
}

double MatlabInt8Value::toDouble()
{
    return m_value;
}


MatlabUint8Value::MatlabUint8Value(unsigned char v)
{
    m_value = v;
}

std::string MatlabUint8Value::getType()
{
    return "Uint8";
}


int MatlabUint8Value::toInt()
{
    return m_value;
}

double MatlabUint8Value::toDouble()
{
    return m_value;
}

MatlabUint16Value::MatlabUint16Value(unsigned short v)
{
    m_value = v;
}

std::string MatlabUint16Value::getType()
{
    return "Uint16";
}


int MatlabUint16Value::toInt()
{
    return m_value;
}

double MatlabUint16Value::toDouble()
{
    return m_value;
}


MatlabDoubleValue::MatlabDoubleValue(double v)
{
    m_value = v;
}

std::string MatlabDoubleValue::getType()
{
    return "Double";
}


int MatlabDoubleValue::toInt()
{
    return m_value;
}

double MatlabDoubleValue::toDouble()
{
    return m_value;
}
