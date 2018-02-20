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
#include "SVMClassifier.h"
#include "File.h"
#include "ReferenceSubImage.h"

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
    m_doExtractHogSvm = false;
    m_doTrainSvmFromFiles = false;
    m_doPredict = false;
    m_doMaxFps = false;
    
    m_doYuv = false;
    m_doMonochrome = false;
    m_doRotateHog = false;
    m_doResizePersons = false;
    m_minPersonHeight = 50;
    m_zoom = 1;
    m_startInFrame = 0;
    m_dataset = "USA";
    m_doResizePersonsX = 64;
    m_doResizePersonsY = 128;
    m_fps = -1;
    
    for (int i=1; i < argc; i++)
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
        else if (strcmp(args[i], "--extract-hog-features") == 0)
            m_doExtractHogFeatures = true;
        else if (strcmp(args[i], "--extract-hog-svm") == 0)
            m_doExtractHogSvm = true;
        else if (strcmp(args[i], "--train-svm-from-files") == 0)
            m_doTrainSvmFromFiles = true;
        else if (strcmp(args[i], "--predict") == 0)
            m_doPredict = true;
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
        else if (strcmp(args[i], "--mono") == 0)
            m_doMonochrome = true;
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
            exit(-1);
        }
    }
}

void PedestrianDetectionBenchmark::validateOptions()
{
    if (m_doPredict && m_doTrainSvmFromFiles)
    {
        printf("[ERROR] Cannot predict and generate SVM model at the same time\n");
        exit(-1);
    }
    if (m_doPredict && !m_doResizePersons)
    {
        printf("[ERROR] Person dimensions must be known for prediction. Use --resize-persons-as\n");
        exit(-1);
    }
    if (m_doPlayInputAsHog && m_doPlayInputSequence)
    {
        printf("[ERROR] Cannot display HOG and original input at the same time\n");
        exit(-1);
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
    
    // Image Extractor initialization
    ImageExtractor extractor;
    extractor.m_resize = m_doResizePersons;
    extractor.m_resizeX = m_doResizePersonsX;
    extractor.m_resizeY = m_doResizePersonsY;
    extractor.m_rotateHog = m_doRotateHog;
    // @todo make it set flexible
    extractor.setExtractionPath((outDir + "/"+m_dataset+"/extraction/set01/V000").c_str());
    
    std::string svmTrainingInput = format("%s/trainingInput.svm", extractor.m_extractionPath.c_str());
    std::string svmModel = format("%s/model.svm", extractor.m_extractionPath.c_str());
    
    // SVM Classifier initialization
    SVMClassifier classifier;

    classifier.setTrainingInputFile(svmTrainingInput);
    classifier.setModelFile(svmModel);
    
    if (header.allocatedFrames != annotationReader.getAnnotatedFrames())
    {
        printf("[ERROR] number of annotations differ from number of images\n");
        exit(-1);
    }
    
    if (m_doTrainSvmFromFiles)
    {
        printf("[INFO] Reading SVM file\n");
        classifier.readProblem();
        classifier.trainModel();
        
        printf("[INFO] Writing SVM model\n");
        classifier.exportModel();
    }
    
    if (m_doPredict)
    {
        printf("[INFO] Loading SVM model\n");
        classifier.importModel();
    }
    
    if (m_doExtractHogSvm)
    {
        if (m_doResizePersons == false)
        {
            printf("[ERROR] SVM classification requires person dimensions to be normalized. Use --resize-persons\n");
            exit(-1);
        }
        classifier.createSvmFile();
    }
    
    //annotationReader.getBoundingBoxes(0);
    
    bool loopFrames = m_doPlayInputSequence | m_doPlayInputAsHog | m_doExtractAnnotatedPersons | m_doExtractHogPersons | m_doExtractHogSvm | m_doPredict;
    
    if (loopFrames)
    {
        XWindow window;
        PerformanceLap lap;
        
        if (m_doPlayInputSequence | m_doPlayInputAsHog | m_doPredict)
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
        {
            reader.skipImageData(&header);
            printf("skiping frame %d\n", i);
        }
        
        for (int i=m_startInFrame; i < header.allocatedFrames; i++)
        {
            printf("Playing frame %d\n", i);
            
            reader.readImageData(&header, &image);
            
            if (m_doYuv)
                image.convertToYuv();
            
            std::vector<BoundingBox> annotatedBoxes = annotationReader.getBoundingBoxes(i);
            std::vector<BoundingBox> centeredBoxes = annotatedBoxes;
            
            if (m_doResizePersons)
            {
                for (int b=0; b < centeredBoxes.size(); b++)
                    centeredBoxes.at(b).centerBox(m_doResizePersonsX, m_doResizePersonsY);
            }
            
            if (m_doPredict | m_doPlayInputSequence)
            {
                window.drawRGBImage(&image);
            }
            
            if (m_doPredict)
            {
                //std::vector<BoundingBox> predictedBoxes = 
                for (int slideY=0; slideY < (image.m_height - m_doResizePersonsY); slideY+=8)
                    for (int slideX=0; slideX < (image.m_width - m_doResizePersonsX); slideX+=8)
                    {
                        //printf("Testing %d %d %d %d\n", slideX, slideY);
                        BoundingBox box(slideX, slideY, m_doResizePersonsX, m_doResizePersonsY, false);
                        ReferenceSubImage subImage(&image, &box);
                        
                        HOGFeature* feature = hogProcess.createFeature(&subImage);
                        double ret = classifier.predict(feature);
                        delete feature;
                        
                        if (ret == 1)
                        {
                            window.drawBoundingBox(box);
                            printf("Found Person at %d %d %d %d\n", slideX, slideY, m_doResizePersonsX, m_doResizePersonsY);
                        }
                    }
            }
            
            if (m_doPlayInputSequence)
            {
                window.drawBoundingBoxes(annotatedBoxes);
            }
            
            if (m_doPredict | m_doPlayInputSequence)
            {
                window.flush();
            }
            
            if (m_doPlayInputAsHog)
            {
                HOGFeature* feature = hogProcess.createFeature(&image);
                Image* hogImage = hogProcess.createHOGVisualization(feature);
                
                if (m_doPlayInputAsHog)
                {
                    window.drawRGBImage(hogImage);
                    window.drawBoundingBoxes(annotatedBoxes);
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
            
            if (m_doExtractAnnotatedPersons | m_doExtractHogPersons | m_doExtractHogFeatures | m_doExtractHogSvm)
            {
                std::vector<Image*> inputPersons;
                
                if (m_doResizePersons)
                {
                    std::vector<Image*> persons = extractor.getPersonsHigher(centeredBoxes, m_minPersonHeight);
                    inputPersons = extractor.resizePersons(persons);
                }
                else
                {
                    inputPersons = extractor.getPersonsHigher(centeredBoxes, m_minPersonHeight);
                }
                
                
                if (m_doExtractAnnotatedPersons)
                {
                    extractor.savePersons(inputPersons);
                }
                if (m_doExtractHogFeatures)
                {
                    std::vector<HOGFeature*> hogs = hogProcess.createHogFeatures(inputPersons);
                    extractor.saveHogFeatures(hogs);                        
                }
                if (m_doExtractHogPersons)
                {
                    std::vector<Image*> hogs = hogProcess.createHogImages(inputPersons);
                    extractor.saveHogPersons(hogs);                        
                }
                if (m_doExtractHogSvm)
                {
                    std::vector<HOGFeature*> hogs = hogProcess.createHogFeatures(inputPersons);
                    classifier.savePersonHogSvms(hogs);
                    hogs.clear();
                    
                    std::vector<Image*> nonPersons = extractor.getNonPersons(centeredBoxes);
                    hogs = hogProcess.createHogFeatures(nonPersons);
                    classifier.saveNonPersonHogSvms(hogs);
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
    printf("--extract-hog-features\n");
    printf("\tExtract the HOG features from the good (non occluded) persons as CSV\n");
    printf("\n");
    printf("--extract-hog-svm\n");
    printf("\tExtract the HOG features from the good (non occluded) persons to be used\n");
    printf("\tas positive input and random crops (without persons) to be used as negative\n");
    printf("\tinput by the training step (also compatible with svm-train from LIBSVM)\n");
    printf("\n");
    printf("--predict\n");
    printf("\tRun pedestrian detection by using the SVM model\n");
    printf("\n");
    printf("--train-svm-from-files\n");
    printf("\tRead svm extracted data from SVM files and create a SVM model\n");
    printf("\tIt expects the files pos_svm.svm and neg_svm.svm for every dataset\n");
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
    printf("--resize-persons-as <x> <y>\n");
    printf("\tResize persons to x * y resolution. Used in conjuntion with \n");
    printf("\t--extract-annotated-persons \n");
    printf("--yuv\n");
    printf("\tConvert the RGB to YUV\n");
    printf("--mono\n");
    printf("\tConvert the RGB to grey scale\n");
    printf("\n");
    
}

