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
 * File:   Downloader.cpp
 * Author: dcr
 * 
 * Created on February 1, 2018, 11:33 AM
 */

#include "Downloader.h"
#include "File.h"

Downloader::Downloader()
{
}

Downloader::Downloader(const Downloader& orig)
{
}

Downloader::~Downloader()
{
}

void Downloader::setURLBase(const char* base)
{
    m_urlBase = base;
}

/**
 * @todo do not hardcode the path
 */
void Downloader::fetchAnnotations()
{
    File annotationsDir((m_outDir + "/" + m_dataSet + "/annotations").c_str());
    
    if (!annotationsDir.exists())
    {
        downloadIfNeeded("annotations.zip");
        decompress((m_outDir + "/" + m_dataSet + "/annotations.zip").c_str());
        remove((m_outDir + "/" + m_dataSet + "/annotations.zip").c_str());
    }
    else
        printf("[INFO] Annotations already fetched\n");
}

void Downloader::fetchSet1()
{
    File set01Dir(("data/"+m_dataSet+"/set01").c_str());
    
    if (!set01Dir.exists())
    {
        downloadIfNeeded("set01.tar");
        decompress(("data/"+m_dataSet+"/set01.tar").c_str());
        remove(("data/"+m_dataSet+"/set01.tar").c_str());
    }
    else
        printf("[INFO] Set01 already fetched\n");
}

void Downloader::remove(const char* file)
{
    std::string cmd = "rm ";
    cmd.append(file);
    //cmd.append(" -d ");
    
    printf("[EXE]");
    printf(cmd.c_str());
    printf("\n");
    
    system(cmd.c_str());
}

bool endsWithCaseInsensitive(const std::string& value, const std::string& ending) 
{
    if (ending.size() > value.size()) {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin(),
        [](const char a, const char b) {
            return tolower(a) == tolower(b);
        }
    );
}
void Downloader::decompress(const char* file)
{
    std::string cmd;
    
    if (endsWithCaseInsensitive(std::string(file), std::string(".zip")))
    {
        cmd = "unzip ";
        cmd.append(file);
        cmd.append(" -d ");
        cmd.append(m_outDir);
    }
    else if (endsWithCaseInsensitive(std::string(file), std::string(".tar")))
    {
        cmd = "tar -xvf ";
        cmd.append(file);
        cmd.append(" -C ");
        cmd.append(m_outDir);
    }
    else
    {
        fprintf(stderr, "unsupported compression format for file %s", file);
        exit(-1);
    }
    
    
    
    printf("[EXE]");
    printf(cmd.c_str());
    printf("\n");
    
    system(cmd.c_str());
}

void Downloader::downloadIfNeeded(const char* file)
{
    std::string finalPath = m_outDir + "/" + m_dataSet + "/" + file;
    
    File check(finalPath);
    
    if (!check.exists())
        download(file);
    else
        printf("[INFO] %s was already downloaded\n", file);
}

void Downloader::download(const char* file)
{
    std::string cmd = "wget ";
    
    cmd.append(m_urlBase);
    cmd.append(file);
    cmd.append(" -P ");
    cmd.append(m_outDir + "/" + m_dataSet);
    
    printf("[EXE]: ");
    printf(cmd.c_str());
    printf("\n");
    
    system(cmd.c_str());
}


void Downloader::setOutputDir(const char* dir)
{
    File file(dir);
    if (!file.exists())
        file.mkdirs();
    
    m_outDir = dir;
}

void Downloader::setDataset(const char* dataset)
{
    m_dataSet = dataset;
}

