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
 * File:   File.h
 * Author: dcr
 *
 * Created on February 1, 2018, 11:45 AM
 */

#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>

class File {
public:
    File(const char* path);
    File(File* parent, const char* path);
    File(std::string& path);
    File(const File& orig);
    virtual ~File();
    
public:
    File* getParentFile();
    bool exists();
    void mkdirs();
    std::vector<File> listFiles();
    
    std::string getPath();
    
private:
    std::string m_path;
};

#endif /* FILE_H */

