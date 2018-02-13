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
 * File:   Downloader.h
 * Author: dcr
 *
 * Created on February 1, 2018, 11:33 AM
 */

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

class Downloader {
public:
    Downloader();
    Downloader(const Downloader& orig);
    virtual ~Downloader();
    
public:
    void setURLBase(const char* base);
    void download(const char* url);
    void downloadIfNeeded(const char* file);
    void setOutputDir(const char* dir);
    void setDataset(const char* dataset);
    
    void fetchAnnotations();
    void fetchSet1();
    void decompress(const char* file);
    void remove(const char* file);
    
private:
    std::string m_urlBase;
    std::string m_outDir;
    std::string m_dataSet;  // the data set (can be USA, INRIA, etc.)
};

#endif /* DOWNLOADER_H */

