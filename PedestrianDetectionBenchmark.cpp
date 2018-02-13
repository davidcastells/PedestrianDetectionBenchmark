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
 * File:   PedestrianDetectionBenchmark.cpp
 * Author: dcr
 * 
 * Created on February 1, 2018, 11:04 AM
 */

#include "PedestrianDetectionBenchmark.h"
#include "Downloader.h"
#include "SeqFileReader.h"
#include "SeqFileHeader.h"
#include "XWindow.h"
#include "MatfileReader.h"
#include "PerformanceLap.h"
#include "BoundingBox.h"
#include "ImageExtractor.h"
#include "BufferedImage.h"
#include "HOGProcessor.h"
#include "HOGFeature.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>


PedestrianDetectionBenchmark::PedestrianDetectionBenchmark()
{
    
}

PedestrianDetectionBenchmark::~PedestrianDetectionBenchmark()
{
    
}



void PedestrianDetectionBenchmark::parseOptions(int argc, char* args[])
{
    m_doUsage = false;
    m_doPlayInputSequence = false;
    m_doPlayInputAsHog = false;
    m_doExtractFrames = false;
    m_doExtractAnnotatedPersons = false;
    m_doExtractHogPersons = false;
    m_doMaxFps = false;
    
    m_doYuv = false;
    m_doRotateHog = false;
    m_doResizePersons = false;
    m_minPersonHeight = 50;
    m_zoom = 1;
    m_startInFrame = 0;
    m_dataset = "USA";
    m_doResizePersonsX = 64;
    m_doResizePersonsY = 128;
    m_fps = -1;
    
    for (int i=0; i < argc; i++)
    {
        if (strcmp(args[i], "--help") == 0)
            m_doUsage = true;
        else if (strcmp(args[i], "--play-input-sequence") == 0)
            m_doPlayInputSequence = true;
        else if (strcmp(args[i], "--play-input-as-hog") == 0)
            m_doPlayInputAsHog = true;
        else if (strcmp(args[i], "--extract-frames") == 0)
            m_doExtractFrames = true;
        else if (strcmp(args[i], "--extract-annotated-persons") == 0)
            m_doExtractAnnotatedPersons = true;
        else if (strcmp(args[i], "--extract-hog-persons") == 0)
            m_doExtractHogPersons = true;
        else if (strcmp(args[i], "--resize-persons-as") == 0)
        {
            m_doResizePersons = true;
            m_doResizePersonsX = atoi(args[++i]);
            m_doResizePersonsY = atoi(args[++i]);
        }
        else if (strcmp(args[i], "--max-fps") == 0)
            m_doMaxFps = true;
        else if (strcmp(args[i], "--yuv") == 0)
            m_doYuv = true;
        else if (strcmp(args[i], "--rotate-hog") == 0)
            m_doRotateHog = true;
        else if (strcmp(args[i], "--dataset") == 0)
        {
            m_dataset = args[++i];
            
            // @todo check valid dataset
        }
        else if (strcmp(args[i], "--min-person-height") == 0)
        {
            m_minPersonHeight = atoi(args[++i]);
            printf("Min Person Height=%d\n", m_minPersonHeight);
        }
        else if (strcmp(args[i], "--start-in-frame") == 0)
        {
            m_startInFrame = atoi(args[++i]);
            printf("Start in frame %d\n", m_startInFrame);
        }
        else if (strcmp(args[i], "--zoom") == 0)
        {
            m_zoom = atoi(args[++i]);
            printf("Zoom=%d\n", m_zoom);
        }
        else if (strcmp(args[i], "--fps") == 0)
        {
            m_fps = atof(args[++i]);
            printf("FPS: %lf\n", m_fps);
        }
        else
        {
            printf("[WARNING] Unrecognized option= %s\n", args[i]);
        }
    }
}

void PedestrianDetectionBenchmark::run()
{
    if (m_doUsage)
    {
        usage();
        exit(0);
    }
    
    Downloader downloader;
    
    std::string outDir = "data";
    
    //downloader.setURLBase("http://www.vision.caltech.edu/Image_Datasets/CaltechPedestrians/datasets/USA/");
    downloader.setURLBase("http://www.vision.caltech.edu/Image_Datasets/CaltechPedestrians/datasets/");
    downloader.setOutputDir(outDir.c_str());
    downloader.setDataset(m_dataset.c_str());
    
    
    
    downloader.fetchAnnotations();
    downloader.fetchSet1();
    
    SeqFileHeader header;
    SeqFileReader reader((outDir +"/"+m_dataset+"/set01/V000.seq").c_str());
    reader.readHeader(&header);
    
    MatfileReader annotationReader((outDir + "/"+m_dataset+"/annotations/set01/V000.vbb").c_str());
    annotationReader.setVerbose(0);
    annotationReader.readVdd();
    
    HOGProcessor hogProcess;
    hogProcess.m_rotateHog = m_doRotateHog;
    
    ImageExtractor extractor;
    extractor.m_resize = m_doResizePersons;
    extractor.m_resizeX = m_doResizePersonsX;
    extractor.m_resizeY = m_doResizePersonsY;    
    extractor.setExtractionPath((outDir + "/"+m_dataset+"/extraction/set01/V000").c_str());
    
    if (header.allocatedFrames != annotationReader.getAnnotatedFrames())
    {
        printf("[ERROR] number of annotations differ from number of images\n");
        exit(-1);
    }
    
    //annotationReader.getBoundingBoxes(0);
    
    bool loopFrames = m_doPlayInputSequence | m_doPlayInputAsHog | m_doExtractAnnotatedPersons | m_doExtractHogPersons;
    
    if (loopFrames)
    {
        XWindow window;
        PerformanceLap lap;
        
        if (m_doPlayInputSequence | m_doPlayInputAsHog)
            window.create(header.width, header.height, header.bitDepth, m_zoom);
        
        if (m_fps == -1)
            m_fps = header.suggestedFrameRate ;
        
        double framePeriodSeconds = 1/m_fps;
        //double framePeriodMicroseconds = framePeriodSeconds * 1E6;
        
        assert(header.bitDepth/8 == 3);
        
        BufferedImage image(header.width, header.height);
                
        double nextDeadline = lap.dtime() + framePeriodSeconds;
        
        // sequence file is sequential (not random access as matlab structure), 
        // so we must read and discard skiped frames
        for (int i=0; i < m_startInFrame; i++)
            reader.skipImageData(&header);
        
        for (int i=m_startInFrame; i < header.allocatedFrames; i++)
        {
            printf("Playing frame %d\n", i);
            
            reader.readImageData(&header, &image);
            
            if (m_doYuv)
                image.convertToYuv();
            
            std::vector<BoundingBox> boxes = annotationReader.getBoundingBoxes(i);
            std::vector<BoundingBox> centeredBoxes = boxes;
            
            if (m_doResizePersons)
            {
                for (int b=0; b < centeredBoxes.size(); b++)
                    centeredBoxes.at(b).centerBox(m_doResizePersonsX, m_doResizePersonsY);
            }
            
            if (m_doPlayInputSequence)
            {
                window.drawRGBImage(&image);
                window.drawBoundingBoxes(boxes);
                window.flush();
            }
            
            if (m_doPlayInputAsHog)
            {
                HOGFeature* feature = hogProcess.createFeature(&image);
                Image* hogImage = hogProcess.createHOGVisualization(feature);
                
                if (m_doPlayInputAsHog)
                {
                    window.drawRGBImage(hogImage);
                    window.drawBoundingBoxes(boxes);
                    window.flush();
                }
                delete feature;
                delete hogImage;
            }
            
            extractor.setFrameNumber(i);
            extractor.setImage(&image);
                
            if (m_doExtractFrames)
            {
                extractor.extractFrame();
            }
            
            if (m_doExtractAnnotatedPersons | m_doExtractHogPersons)
            {
                if (m_doResizePersons)
                {
                    std::vector<Image*> persons = extractor.getPersonsHigher(centeredBoxes, m_minPersonHeight);
                    std::vector<Image*> resized = extractor.resizePersons(persons);
                    
                    if (m_doExtractAnnotatedPersons)
                    {
                        extractor.savePersons(resized);
                    }
                    if (m_doExtractHogPersons)
                    {
                        std::vector<Image*> hogs = hogProcess.createHogImages(resized);
                        extractor.saveHogPersons(hogs);                        
                    }
                }
                else
                {
                    std::vector<Image*> persons = extractor.getPersonsHigher(boxes, m_minPersonHeight);
                    extractor.savePersons(persons);
                }
            }

            
            if (!m_doMaxFps)
            {
                while (lap.dtime() < nextDeadline) ;    // busy wait
             
                nextDeadline = lap.dtime() + framePeriodSeconds;
            }
        }
    }
}

void PedestrianDetectionBenchmark::usage()
{
    printf("Usage:\n");
    printf("pedestriandetectionbenchmak [options]\n");
    printf("\n");
    printf("where [options] can be\n");
    printf("\n");
    printf("--help\n");
    printf("\tshows this message\n");
    printf("\n");
    printf("--play-input-sequence\n");
    printf("\tDisplay the input images\n");
    printf("\n");
    printf("--play-input-as-hog\n");
    printf("\tDisplay the input images as HOG features\n");
    printf("\n");
    printf("--rotate-hog\n");
    printf("\tRotates the visualization of the HOG features by 90º for \n");
    printf("\tmore intuitive interpretation of the scene ");
    printf("\n");
    printf("--extract-frames\n");
    printf("\tExtract individual frames\n");
    printf("\n");
    printf("--min-person-height <n>\n");
    printf("\tMinimum height of the considered persons\n");
    printf("\n");
    printf("--extract-annotated-persons\n");
    printf("\tExtract the pictures from the good (non occluded) persons\n");
    printf("\n");
    printf("--extract-hog-persons\n");
    printf("\tExtract the pictures from the good (non occluded) persons as HOG\n");
    printf("\n");    
    printf("--zoom <n>\n");
    printf("\tSets the window zoom\n");
    printf("\n");
    printf("--start-in-frame <n>\n");
    printf("\tStart processing in frame <n>\n");
    printf("\n");
    printf("--dataset <name>\n");
    printf("\tSelect the dataset to work with. Supported <name>s are USA, INRIA\n");
    printf("\n");
    printf("\n");
    printf("--fps <n>\n");
    printf("\tPlayback frames per second\n");
    printf("\n");
    printf("--resize-person-as <x> <y>\n");
    printf("\tResize persons to x * y resolution. Used in conjuntion with \n");
    printf("\t--extract-annotated-persons \n");
    printf("--yuv\n");
    printf("\tConvert the RGB to YUV\n");
    printf("\n");
    
}

