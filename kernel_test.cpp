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
 * File:   main.c
 * Author: dcr
 *
 * Created on February 1, 2018, 10:59 AM
 */

#include <stdio.h>
#include <stdlib.h>

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
#include "FPGAHOGProcessor.h"
#include "HOGFeature.h"
#include "SVMClassifier.h"
#include "File.h"
#include "ReferenceSubImage.h"
#include "ImageIO.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <unistd.h>

class KernelTest
{
public:
    KernelTest();
    virtual ~KernelTest();
    
public:
    void run();
    void parseOptions(int argc, char* args[]);
    void slidingWindowPrediction(Image* refImage, std::vector<BoundingBox>& annotatedBoxes);
    
private:
    std::string m_dataset;
    bool m_doHeadless;
    int m_startInFrame;
    bool m_doMonochrome;
    bool m_doMaxFps;
    bool m_doPredict;
    int m_doResizePersonsX;
    int m_doResizePersonsY;
    double m_multiscaleIncFactor;
    int m_multiscales;

private:
    Downloader downloader;
    MatfileReader annotationReader;
    FPGAHOGProcessor hogProcess;

};

/*
 * 
 */
int main(int argc, char** argv)
{
    KernelTest test;
    test.parseOptions(argc, argv);
    test.run();
}



KernelTest::KernelTest()
{
    
}

KernelTest::~KernelTest()
{
    
}



void KernelTest::parseOptions(int argc, char* args[])
{
    m_dataset = "USA";
    m_doHeadless = true;
    m_startInFrame = 0;
    m_doMonochrome = true;
    m_doMaxFps = true;
    m_doPredict = true;
    m_doResizePersonsX = 96;
    m_doResizePersonsY = 160;

    m_multiscaleIncFactor = 1/1.2;
    m_multiscales = 12;

//    m_multiscaleIncFactor = 1.1;
//    m_multiscales = 15;
    
//    m_invocationPath = args[0];
    
    for (int i=1; i < argc; i++)
    {
//        if (strcmp(args[i], "--help") == 0)
//            m_doUsage = true;
        
    }
}


/**
 * 
 * @param image
 * @param annotatedBoxes
 */
void KernelTest::slidingWindowPrediction(Image* refImage, std::vector<BoundingBox>& annotatedBoxes)
{
    bool extractedNegatives = false;
    
    // We keep a vector of notFound people boxes, so that at the end we can 
    // get a list of False Negatives
    std::vector<BoundingBox> notFound;

    // fill the not found vector
    for (int i=0; i < annotatedBoxes.size(); i++)
        notFound.push_back(annotatedBoxes[i]);

    double curW = refImage->m_width;
    double curH = refImage->m_height;
    double scaleFactor = 1;
    
    int resizeXInBlocks = 2 * ((m_doResizePersonsX / 8) / 2) - 1;
    int resizeYInBlocks = 2 * ((m_doResizePersonsY / 8) / 2) - 1;


    double scale_resize_lap[m_multiscales];
    double scale_gradient_lap[m_multiscales];
    double scale_histogram_lap[m_multiscales];
    double scale_norm_lap[m_multiscales];

    
    PerformanceLap multiscaleLap;
    multiscaleLap.start();
    
    for (int scaleIndex=0; scaleIndex < m_multiscales; scaleIndex++)
    {        
        //printf("scale: %f (%d, %d, %d)\n", scaleFactor, (int)curW, (int)curH, refImage->m_channels);

        PerformanceLap tResize;
        BufferedImage* image = new BufferedImage(curW, curH, refImage->m_channels);
        image->resizeFrom(refImage);
        tResize.stop();
        scale_resize_lap[scaleIndex] = tResize.lap();
        
        //printf("resized\n");
//        XWindow debugWindow;
//        
//        debugWindow.create(curW, curH, 32, 1);
//        debugWindow.drawRGBImage(image);
        HOGFeature* wholeImageFeature = hogProcess.allocateMem(image);

        //printf("mem allocated\n");
        
        PerformanceLap tHog;
        hogProcess.computeGradient(image);
        tHog.stop();
        scale_gradient_lap[scaleIndex] = tHog.lap();


        PerformanceLap tHis;
        hogProcess.computeHistogram(image, wholeImageFeature);
        tHis.stop();
        scale_histogram_lap[scaleIndex] = tHis.lap();

        PerformanceLap tNorm;
        hogProcess.computeNorm(image, wholeImageFeature);
        tNorm.stop();
        scale_norm_lap[scaleIndex] = tNorm.lap();

        
//        printf("nobx=%d\n", wholeImageFeature->getOverlappingBlocksInAxisX());
//        printf("noby=%d\n", wholeImageFeature->getOverlappingBlocksInAxisY());
//        double tTotalPredict = 0;
//        
//        for (int blocky=0; blocky < wholeImageFeature->getOverlappingBlocksInAxisY(); blocky++)
//            for (int blockx=0; blockx < wholeImageFeature->getOverlappingBlocksInAxisX(); blockx++)
//            {
//                int slideX = wholeImageFeature->getImageXFromCellIndexX(blockx, 0);
//                int slideY = wholeImageFeature->getImageYFromCellIndexY(blocky, 0);
//                
//                if (slideX >= (image->m_width - m_doResizePersonsX))
//                    continue;
//                if (slideY >= (image->m_height - m_doResizePersonsY))
//                    continue;
//                
////                printf("Testing %d %d %d %d\n", slideX, slideY, m_doResizePersonsX, m_doResizePersonsY);
//                
//                BoundingBox box(slideX, slideY, m_doResizePersonsX, m_doResizePersonsY, false);
//                
//                
//                // HOGFeature* feature = hogProcess.createFeature(&subImage);
//                HOGFeature* feature = wholeImageFeature->createWindow(blockx, blocky, resizeXInBlocks, resizeYInBlocks);
//
//                PerformanceLap tPredict;
//                double ret = 0;
////                double ret = classifier.predict(feature);
//                tTotalPredict += tPredict.lap();
//                delete feature;
//
//                if (ret > 0.7)
//                {
//                    BoundingBox scaledImageBox(box);
//                    box.scaleDown(scaleFactor);         // downsize to draw the box in the original image
//                    
////                    if (!m_doHeadless)
////                    {
////                        window.drawBoundingBox(box);
////                        window.flush();
////                    }
//                    printf("Score: %0.2f - Found Person at %d %d %d %d - Scale: %d (%f)\n", ret, 
//                            (int) box.m_x, (int) box.m_y, (int) box.m_width, (int) box.m_height, scaleIndex, scaleFactor);
//
//                    if (box.collide(annotatedBoxes))
//                    {                 
//                        // found bos
//                        // remove this box from the notfound list
//                        int collisionIndex = box.collisionIndex(notFound);
//
//                        if (collisionIndex != -1)
//                            notFound.erase(notFound.begin()+collisionIndex);
//                        
//                    }
//                    else
//                    {
////                        // 
////                        if (m_doExtractMispredictedSvm)
////                        {
////                            // the box is identified as person, but does not colide with any annotation,
////                            // add it as a negative sample
////                            
////                            // we have to extract from the scaled image now
////                            ReferenceSubImage subImage(image, &scaledImageBox);
////                            extractor.saveSvmTraining(&subImage, false);
////                            extractedNegatives = true;
////                            //exit(0);
////                            
////                        }
//                    }
//                }
//            }
//        
        
//        printf("Predict time for %d SVs: %f\n", classifier.getNumSupportVectors(), tTotalPredict);
        
        scaleFactor *= m_multiscaleIncFactor;
        int roundingX = hogProcess.m_cellWidth * hogProcess.m_blockWidth;
        int roundingY = hogProcess.m_cellHeight * hogProcess.m_blockHeight;
        curW = (int) ((refImage->m_width * scaleFactor + (roundingX-1)) / roundingX) * roundingX;
        curH = (int) ((refImage->m_height * scaleFactor + (roundingY-1)) / roundingY) * roundingY;
        
        delete image;
    }
    
    multiscaleLap.stop();
    
    double totalResizeLap = 0;
    double totalGradientLap = 0;
    double totalHistogramLap = 0;
    double totalNormLap = 0;
    
    for (int scaleIndex=0; scaleIndex < m_multiscales; scaleIndex++)
    {
        double fpsResize = 1/scale_resize_lap[scaleIndex];
        totalResizeLap += scale_resize_lap[scaleIndex];

        double fpsGradient = 1/scale_gradient_lap[scaleIndex];
        totalGradientLap += scale_gradient_lap[scaleIndex];

        double fpsHistogram = 1/scale_histogram_lap[scaleIndex];
        totalHistogramLap += scale_histogram_lap[scaleIndex];
        
        double fpsNorm = 1/scale_norm_lap[scaleIndex];
        totalNormLap += scale_norm_lap[scaleIndex];

        printf("Scale[%d] Resize time: %f seconds  %0.2f FPS - Gradient time: %f seconds  %0.2f FPS - Histogram time: %f seconds  %0.2f FPS - Norm time: %f seconds  %0.2f FPS\n",
                scaleIndex, scale_resize_lap[scaleIndex], fpsResize, scale_gradient_lap[scaleIndex], fpsGradient, 
                scale_histogram_lap[scaleIndex], fpsHistogram, scale_norm_lap[scaleIndex], fpsNorm);
    }

    printf("TOTAL Resize time: %f seconds  %0.2f FPS - Gradient time: %f seconds  %0.2f FPS - Histogram time: %f seconds  %0.2f FPS - Norm time: %f seconds  %0.2f FPS\n", 
        totalResizeLap, 1/totalResizeLap, totalGradientLap, 1/totalGradientLap, totalHistogramLap, 1/totalHistogramLap, totalNormLap, 1/totalNormLap);
    
    printf("TOTAL Multiscale: %f seconds %0.2f FPS\n", multiscaleLap.lap(), 1/multiscaleLap.lap());
//    if (m_doStopAfterMisprediction)
//        if (extractedNegatives)
//            exit(0);
    
//    for (int i=0; i < notFound.size(); i++)
//    {
//        BoundingBox box = notFound[i];
//        printf("Score: 0 - Missed Person at %f %f %f %f\n", box.m_x, box.m_y, box.m_width, box.m_height);
//        
//        if (m_doExtractMispredictedSvm)
//        {
//            // the box was not identified as person, 
//            // add it as a positive sample
//            ReferenceSubImage subImage(refImage, &box);
//            BufferedImage bufImg(m_doResizePersonsX, m_doResizePersonsY, refImage->m_channels);
//
//            bufImg.resizeFrom(&subImage);
//
//            extractor.saveSvmTraining(&bufImg, true);
//            if (m_doStopAfterMisprediction)
//                exit(0);
//        }
//    }
}


void KernelTest::run()
{
//    if (m_doUsage)
//    {
//        usage();
//        exit(0);
//    }
    
    std::string outDir = "data";

    // Initialize downloader
    //downloader.setURLBase("http://www.vision.caltech.edu/Image_Datasets/CaltechPedestrians/datasets/USA/");
    downloader.setURLBase("http://www.vision.caltech.edu/Image_Datasets/CaltechPedestrians/datasets/");
    downloader.setOutputDir(outDir.c_str());
    downloader.setDataset(m_dataset.c_str());
    
    downloader.fetchAnnotations();
    downloader.fetchSet1();
    
    SeqFileHeader header;
    SeqFileReader reader((outDir +"/"+m_dataset+"/set01/V000.seq").c_str());
    reader.readHeader(&header);
    
    annotationReader = MatfileReader((outDir + "/"+m_dataset+"/annotations/set01/V000.vbb").c_str());
    annotationReader.setVerbose(0);
    annotationReader.readVdd();
    
    if (header.allocatedFrames != annotationReader.getAnnotatedFrames())
    {
        printf("[ERROR] number of annotations differ from number of images\n");
        exit(-1);
    }
    
//    
//    
//    hogProcess.m_rotateHog = m_doRotateHog;
//    
//    // Image Extractor initialization
//    
//    extractor.m_resize = m_doResizePersons;
//    extractor.m_resizeX = m_doResizePersonsX;
//    extractor.m_resizeY = m_doResizePersonsY;
//    extractor.m_rotateHog = m_doRotateHog;
//    // @todo make it set flexible
//    extractor.setExtractionPath((outDir + "/"+m_dataset+"/extraction/set01/V000").c_str());
//    
//    std::string svmTrainingInput = format("%s/trainingInput.svm", extractor.m_extractionPath.c_str());
//    std::string svmModel = format("%s/model.svm", extractor.m_extractionPath.c_str());
//    
//    // SVM Classifier initialization
//
//    classifier.setTrainingInputFile(svmTrainingInput);
//    classifier.setModelFile(svmModel);
//    
//    
//    if (m_doShowPositiveSamples)
//    {
//        showPositiveSamples();
//        exit(0);
//    }
//    
//    if (m_doTrainSvmFromFiles)
//    {
//        printf("[INFO] Reading SVM file\n");
//        classifier.readProblem();
//        printf("[INFO] Training Model\n");
//        classifier.trainModel();
//        
//        printf("[INFO] Writing SVM model\n");
//        classifier.exportModel();
//    }
//    
//    if (m_doPredict | m_doPredictExtractedImages)
//    {
//        printf("[INFO] Loading SVM model\n");
//        classifier.importModel();
//        classifier.configureFeatureDimension(m_doResizePersonsX, m_doResizePersonsY, m_doMonochrome);
//    }
//    
//    if (m_doExtractHogSvm | m_doExtractSvmFromImages)
//    {
//        classifier.createSvmFile();
//    }
//    
//    if (m_doExtractSvmFromImages)
//    {
//        generateSvmInputFromExtractedImages();
//    }
//    
//    if (m_doPredictExtractedImages)
//    {
//        printf("[INFO] Creating SVM input file from extracted images\n");
//
//        File dir(extractor.m_extractionPath);
//
//        std::vector<File> files = dir.listFiles();
//
//        XWindow debugWindow;
//        debugWindow.create(m_doResizePersonsX, m_doResizePersonsY, 24, 2);
//        
//        for (int i=0; i < files.size(); i++)
//        {
//            std::string path = files.at(i).getPath();
//            //printf("File %d = %s\n", i, files.at(i).getPath().c_str());
//
//            bool isNeg = path.find("svm_neg") != std::string::npos;
//            bool isPos = path.find("svm_pos") != std::string::npos;
//
//            if (isNeg | isPos)
//            {
//                Image* image = ImageIO::loadImage(path.c_str(), (m_doMonochrome)? 1 : 3);
//                
//                debugWindow.drawRGBImage(image);
//                debugWindow.flush();
//                
//                HOGFeature* feature = hogProcess.createFeature(image);
//
//                double v = classifier.predict(feature);
//
//                printf("Predict %s [%s]: %0.2f ", path.c_str() , (isPos)?"+":"-", v);
//
//                bool isOk = (isNeg && v < 0.5) || (isPos && v > 0.5);
//
//                printf(isOk? "[OK]":"##### error #####");
//		printf(" features: %d", feature->getTotalBins());
//
//                printf("\n");
//                delete feature;
//                delete image;
//            }
//        }    
//    }
    
//    bool loopFrames = m_doPlayInputSequence | m_doPlayInputAsHog | m_doExtractAnnotatedPersons | m_doExtractHogPersons | m_doExtractImagesSvm | m_doExtractHogSvm | m_doPredict;
    
    bool loopFrames = true;
    
    if (loopFrames)
    {
        
        PerformanceLap lap;
        
//        if (m_doPlayInputSequence | m_doPlayInputAsHog | m_doPredict) 
//            if (!m_doHeadless)
//                window.create(header.width, header.height, header.bitDepth, m_zoom);
        
//        if (m_fps == -1)
//            m_fps = header.suggestedFrameRate ;
        
//        double framePeriodSeconds = 1/m_fps;
        //double framePeriodMicroseconds = framePeriodSeconds * 1E6;
        
        assert(header.bitDepth/8 == 3);
        
        BufferedImage image(header.width, header.height);
                
//        double nextDeadline = lap.dtime() + framePeriodSeconds;
        
        // sequence file is sequential (not random access as matlab structure), 
        // so we must read and discard skiped frames
//        for (int i=0; i < m_startInFrame; i++)
//        {
//            reader.skipImageData(&header);
//            printf("skiping frame %d\n", i);
//        }
        
        for (int i=m_startInFrame; i < header.allocatedFrames; i++)
        {
            printf("Playing frame %d\n", i);
            
//            extractor.setFrameNumber(i);
            
            reader.readImageData(&header, &image);
            
//            if (m_doYuv)
//                image.convertToYuv();
//            else
                if (m_doMonochrome)
                image.convertToMonochrome();
                
            std::vector<BoundingBox> annotatedBoxes = annotationReader.getBoundingBoxes(i);
            std::vector<BoundingBox> centeredBoxes = annotatedBoxes;
            
//            if (m_dumpAnnotationInfo)
//            {
//                for (int b=0; b < annotatedBoxes.size(); b++)
//                {
//                    BoundingBox box = annotatedBoxes.at(b);
//                    
//                    printf("Box:  pos (%d,%d)  dim (%d,%d) Id %d Occluded %d V: pos (%f,%f)  dim (%f,%f)\n", 
//                            (int) box.m_x, (int)box.m_y, (int) box.m_width, (int) box.m_height,
//                            box.m_objId, box.m_occluded,
//                            box.m_vx, box.m_vy, box.m_vw, box.m_vh);
//                }
//            }
            
            
            
//            if (m_doResizePersons)
//            {
//                for (int b=0; b < centeredBoxes.size(); b++)
//                {
//                    BoundingBox box = centeredBoxes.at(b);
//                    
//                    if (box.m_width >= m_minPersonWidth)
//                    {
//                        // Avoid centering (expanding the width) of persons lower than 
//                        // the threshold so that they can be later filtered out
//                        box.centerBox(m_doResizePersonsX, m_doResizePersonsY);
//                        centeredBoxes.at(b) = box;
//                    }
//                }
//                
//                if (m_dumpAnnotationInfo)
//                {
//                    for (int b=0; b < centeredBoxes.size(); b++)
//                    {
//                        BoundingBox box = centeredBoxes.at(b);
//
//                        printf("Centered Box:  pos (%d,%d)  dim (%d,%d) Id %d Occluded %d V: pos (%f,%f)  dim (%f,%f)\n", 
//                                (int) box.m_x, (int)box.m_y, (int) box.m_width, (int) box.m_height,
//                                box.m_objId, box.m_occluded,
//                                box.m_vx, box.m_vy, box.m_vw, box.m_vh);
//                    }
//                }
//            }
            
            
            
//            if (m_doPredict | m_doPlayInputSequence)
//            {
//                if (!m_doHeadless)
//                    window.drawRGBImage(&image);
//            }
            
            BufferedImage* bigImage = new BufferedImage(640*3, 480*3, image.m_channels);
            bigImage->resizeFrom(&image);

            BufferedImage* wideImage = new BufferedImage(640*3, 1080, image.m_channels);
            wideImage->cropFrom(bigImage, 0, 480*3-1080);
            delete bigImage;
            
            
            if (m_doPredict)
            {
                slidingWindowPrediction(wideImage, annotatedBoxes);
            }

            delete wideImage;

            
//            if (m_doPlayInputSequence)
//            {
//                if (!m_doHeadless)
//                    if (m_doDrawCenteredBoxes)
//                        window.drawBoundingBoxes(centeredBoxes);
//                    else
//                        window.drawBoundingBoxes(annotatedBoxes);
//            }
//            
//            if (m_doPredict | m_doPlayInputSequence)
//            {
//                if (!m_doHeadless)
//                    window.flush();
//            }
            
//            if (m_doPlayInputAsHog)
//            {
//                HOGFeature* feature = hogProcess.createFeature(&image);
//                Image* hogImage = hogProcess.createHOGVisualization(feature);
//                
//                if (m_doPlayInputAsHog)
//                {
//                    if (!m_doHeadless)
//                    {
//                        window.drawRGBImage(hogImage);
//                        window.drawBoundingBoxes(annotatedBoxes);
//                        window.flush();
//                    }
//                }
//                delete feature;
//                delete hogImage;
//            }
            
            
//            extractor.setImage(&image);
//                
//            if (m_doExtractFrames)
//            {
//                extractor.extractFrame();
//            }
//            
//            if (m_doExtractAnnotatedPersons | m_doExtractHogPersons | m_doExtractHogFeatures | m_doExtractImagesSvm | m_doExtractHogSvm)
//            {
//                std::vector<Image*> inputPersons;
//                
//                if (m_doResizePersons)
//                {
//                    std::vector<Image*> persons = extractor.getPersonsHigher(centeredBoxes, m_minPersonWidth, m_minPersonHeight);
//                    inputPersons = extractor.resizePersons(persons);
//                }
//                else
//                {
//                    inputPersons = extractor.getPersonsHigher(centeredBoxes, m_minPersonWidth, m_minPersonHeight);
//                }
//                
//                
//                if (m_doExtractAnnotatedPersons)
//                {
//                    extractor.savePersons(inputPersons);
//                }
//                if (m_doExtractHogFeatures)
//                {
//                    std::vector<HOGFeature*> hogs = hogProcess.createHogFeatures(inputPersons);
//                    extractor.saveHogFeatures(hogs);                        
//                }
//                if (m_doExtractHogPersons)
//                {
//                    std::vector<Image*> hogs = hogProcess.createHogImages(inputPersons);
//                    extractor.saveHogPersons(hogs);                        
//                }
//                if (m_doExtractHogSvm | m_doExtractImagesSvm)
//                {
//                    if (inputPersons.size() > 0)
//                    {
//                        std::vector<Image*> nonPersons = extractor.getNonPersons(centeredBoxes);
//
//                        if (m_doExtractImagesSvm)
//                        {
//                            extractor.saveSvmTraining(inputPersons, true);
//                            extractor.saveSvmTraining(nonPersons, false);
//                        }
//                        if (m_doExtractHogSvm)
//                        {                        
//                            std::vector<HOGFeature*> hogs = hogProcess.createHogFeatures(inputPersons);
//                            classifier.savePersonHogSvms(hogs);
//                            hogs.clear();
//
//                            hogs = hogProcess.createHogFeatures(nonPersons);
//                            classifier.saveNonPersonHogSvms(hogs);
//                        }
//                    }
//                }
//            }

            
//            if (!m_doMaxFps)
//            {
//                while (lap.dtime() < nextDeadline) ;    // busy wait
//             
//                nextDeadline = lap.dtime() + framePeriodSeconds;
//            }
        }
    }
    
//    if (m_doExtractHogSvm | m_doExtractSvmFromImages)
//    {
//        printf("[INFO] SVM Data: %d Positive Samples %d Negative Samples\n", classifier.m_positiveSample, classifier.m_negativeSample);
//    }
}

//void KernelTest::usage()
//{
//    printf("Usage:\n");
//    printf("pedestriandetectionbenchmak [options]\n");
//    printf("\n");
//    printf("where [options] can be\n");
//    printf("\n");
//    printf("--help\n");
//    printf("\tshows this message\n");
//    printf("\n");
//    printf("--play-input-sequence\n");
//    printf("\tDisplay the input images\n");
//    printf("\n");
//    printf("--draw-centered-boxes\n");
//    printf("\tDraws the bounding boxes centered w.r.t. the normalized bounding box\n");
//    printf("\n");
//    printf("--play-input-as-hog\n");
//    printf("\tDisplay the input images as HOG features\n");
//    printf("\n");
//    printf("--rotate-hog\n");
//    printf("\tRotates the visualization of the HOG features by 90º for \n");
//    printf("\tmore intuitive interpretation of the scene \n");
//    printf("\n");
//    printf("--extract-frames\n");
//    printf("\tExtract individual frames\n");
//    printf("\n");
//    printf("--min-person-height <n>\n");
//    printf("\tMinimum height of the considered persons\n");
//    printf("\n");
//    printf("--min-person-width <n>\n");
//    printf("\tMinimum width of the considered persons\n");
//    printf("\n");
//    printf("--extract-annotated-persons\n");
//    printf("\tExtract the pictures from the good (non occluded) persons\n");
//    printf("\n");
//    printf("--extract-hog-persons\n");
//    printf("\tExtract the pictures from the good (non occluded) persons as HOG\n");
//    printf("\n");
//    printf("--extract-hog-features\n");
//    printf("\tExtract the HOG features from the good (non occluded) persons as CSV\n");
//    printf("\n");
//    printf("--extract-images-svm\n");
//    printf("\tExtract the HOG images from the good (non occluded) persons to be used\n");
//    printf("\tas positive input and random crops (without persons) to be used as negative\n");
//    printf("\tinput by the training step\n");
//    printf("\n");
//    printf("--extract-hog-svm\n");
//    printf("\tExtract the HOG features from the good (non occluded) persons to be used\n");
//    printf("\tas positive input and random crops (without persons) to be used as negative\n");
//    printf("\tinput by the training step (also compatible with svm-train from LIBSVM)\n");
//    printf("\n");
//    printf("--create-svm-from-extracted-images\n");
//    printf("\tCreate the SVM features from the individual images extracted with the\n");
//    printf("\t--extract-images-svm flag.\n");
//    printf("\n");
//    printf("--extract-mispredicted-svm\n");
//    printf("\tExtract the mispredicted images\n");
//    printf("\n");
//    printf("--stop-after-misprediction\n");
//    printf("\tStop after misprediction so that the model can be retrained\n");
//    printf("\n");    
//    printf("--predict\n");
//    printf("\tRun pedestrian detection by using the SVM model\n");
//    printf("\n");
//    printf("--predict-extracted-images\n");
//    printf("\tRun pedestrian detection on the images used to create the model\n");
//    printf("\n");
//    printf("--train-svm-from-files\n");
//    printf("\tRead svm extracted data from SVM files and create a SVM model\n");
//    printf("\tIt expects the files pos_svm.svm and neg_svm.svm for every dataset\n");
//    printf("\n");
//    printf("--automated-training\n");
//    printf("\tAutomatically trains the model by iterating extraction and prediction\n");
//    printf("\n");
//    printf("--zoom <n>\n");
//    printf("\tSets the window zoom\n");
//    printf("\n");
//    printf("--start-in-frame <n>\n");
//    printf("\tStart processing in frame <n>\n");
//    printf("\n");
//    printf("--dataset <name>\n");
//    printf("\tSelect the dataset to work with. Supported <name>s are USA, INRIA\n");
//    printf("\n");
//    printf("\n");
//    printf("--fps <n>\n");
//    printf("\tPlayback frames per second\n");
//    printf("\n");
//    printf("--resize-persons-as <x> <y>\n");
//    printf("\tResize persons to x * y resolution. Used in conjuntion with \n");
//    printf("\t--extract-annotated-persons \n");
//    printf("--yuv\n");
//    printf("\n");
//    printf("\tConvert the RGB to YUV\n");
//    printf("\n");
//    printf("--mono\n");
//    printf("\tConvert the RGB to grey scale\n");
//    printf("\n");
//    printf("--dump-annotation-info\n");
//    printf("\tDump information about the annotation\n");
//    printf("\n");
//    printf("--show-positive-samples");
//    printf("\tDisplay the positive samples considered by the system\n");
//    printf("\n");
//
//}

