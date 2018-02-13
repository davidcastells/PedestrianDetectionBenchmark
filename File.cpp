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
 * File:   File.cpp
 * Author: dcr
 * 
 * Created on February 1, 2018, 11:45 AM
 */

#include <sys/stat.h>

#include "File.h"

File::File(const char* path)
{
    m_path = path;
}

File::File(std::string& path)
{
    m_path = path;
}

File::File(const File& orig)
{
}

File::~File()
{
}

bool File::exists()
{
    struct stat res;
    int ret = stat(m_path.c_str(), &res);
    
    if (ret == 0)
        return true;
    else
        return false;
}

void File::mkdirs()
{
    File* parent = getParentFile();
    
    if (parent != NULL)
    {
        parent->mkdirs();
        delete parent;
    }
    
    std::string cmd = "mkdir ";
    cmd.append(m_path);
    
    printf("[EXE]: ");
    printf(cmd.c_str());
    printf("\n");
    
    system(cmd.c_str());
}

File* File::getParentFile()
{
    int pos = m_path.find_last_of("/");
    
    if (pos == std::string::npos)
        return NULL;
    
    std::string parent = m_path.substr(0, pos);
    
    return new File(parent.c_str());
}