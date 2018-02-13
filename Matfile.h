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
 * File:   Matfile.h
 * Author: dcr
 *
 * Created on February 6, 2018, 4:41 PM
 */

#ifndef MATFILE_H
#define MATFILE_H

#include <string>
#include <vector>


class MatfileHeader 
{
public:
    std::string description;
    unsigned long long dataOffset;
    int version;
    int endianess;
    
public:
    std::string toString();
};


/**
 * Base class for all Matlab elements
 */
class MatlabDataElement
{
public:
    virtual std::string getType() = 0;
    virtual void dumpInfo() = 0;
public:
    unsigned int m_size;
};


class MatlabValue : public MatlabDataElement
{
public:
    virtual std::string getType() = 0;
    virtual int toInt() = 0;
    virtual double toDouble() = 0;
    

    virtual void dumpInfo();
};


/**
 * Matlab Struct Header
 */
class MatlabStructHeader :  public MatlabDataElement
{
public:
    virtual std::string getType();
    virtual void dumpInfo();
    
public:
    std::vector<std::string> m_fieldNames;
};

/**
 * Matlab Struct Row
 */
class MatlabStructRow :  public MatlabDataElement
{
public:
    virtual std::string getType();
    virtual void dumpInfo();
    
public:
    std::vector<MatlabDataElement*> m_fieldValues;
};


/**
 * 
 * @param v
 */
class MatlabInt8Value : public MatlabValue
{
public:
    virtual std::string getType();
    virtual int toInt();
    virtual double toDouble();
    
public:
    MatlabInt8Value(char v);
    
    char m_value;
};


class MatlabUint8Value : public MatlabValue
{
public:
    virtual std::string getType();
    virtual int toInt();
    virtual double toDouble();
    
public:
    MatlabUint8Value(unsigned char v);
    
    unsigned char m_value;
};

class MatlabUint16Value : public MatlabValue
{
public:
    virtual std::string getType();
    virtual int toInt();
    virtual double toDouble();
    
public:
    MatlabUint16Value(unsigned short v);
    
    unsigned short m_value;
};

/**
 */
class MatlabDoubleValue : public MatlabValue
{
public:
    virtual std::string getType();
    virtual int toInt();
    virtual double toDouble();
    
public:
    MatlabDoubleValue(double v);
    
    double m_value;
};

/**
 * Matlab Matrix
 */
class MatlabMatrix : public MatlabDataElement
{
public:
    MatlabMatrix();
    virtual ~MatlabMatrix();
    
public:
    virtual std::string getType();
    virtual void dumpInfo();
    
    void setDimensions(std::vector<int> dimensions);
    void set(int i, MatlabDataElement* data);
    void set(int x, int y, MatlabDataElement* data);
    MatlabDataElement* get(int i);
    MatlabDataElement* get(int x, int y);
    MatlabDataElement* get(std::string fieldName);
    MatlabDataElement* get(std::string fieldName, int rowNum);
    
    void setStructHeader(MatlabStructHeader* header);
    
    int totalSize();
    int getFieldPos(std::string fieldName);
    
public:
    std::string m_name;
    int m_class;
    std::vector<int> m_dimensions;    
    std::vector<MatlabDataElement*> m_values;
    MatlabStructHeader* m_structHeader;
    
};

/**
 * Matlab Cell
 * @return 
 */
class MatlabCell : public MatlabDataElement
{
public:
    virtual std::string getType();
    virtual void dumpInfo();
    
public:    
    MatlabMatrix* m_matrix;
    
    
};


#endif /* MATFILE_H */

