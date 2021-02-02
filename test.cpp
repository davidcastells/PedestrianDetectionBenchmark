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
 * File:   test.cpp
 * Author: dcr
 *
 * Created on November 23, 2019, 5:37 PM
 */

#include <cstdlib>

#include "Downloader.h"
#include "SeqFileHeader.h"
#include "MatfileReader.h"
#include "ImageExtractor.h"
#include "SeqFileReader.h"
#include "BufferedImage.h"
#include "ReferenceSubImage.h"

using namespace std;

/*
 * This test extracts images
 * 
 * This must be compiled separatelly
 * 
 * g++ test.cpp SeqFileReader.cpp SeqFileHeader.cpp Image.cpp Downloader.cpp File.cpp ImageExtractor.cpp Matfile.cpp MatfileReader.cpp BoundingBox.cpp HOGFeature.cpp BufferedImage.cpp ReferenceSubImage.cpp StringUtils.cpp -o test.exe -ljpeg -lpng -std=c++11
 *
 */
int main(int argc, char** argv) 
{
    std::string outDir = "data";
    std::string m_dataset =  "USA";


    Downloader downloader;

    downloader.setURLBase("http://www.vision.caltech.edu/Image_Datasets/CaltechPedestrians/datasets/");
    downloader.setOutputDir(outDir.c_str());
    downloader.setDataset(m_dataset.c_str());
    
    downloader.fetchAnnotations();
    downloader.fetchSet1();
    
    SeqFileHeader header;
    SeqFileReader reader((outDir +"/"+m_dataset+"/set01/V000.seq").c_str());
    reader.readHeader(&header);
    
    MatfileReader annotationReader;

    annotationReader = MatfileReader((outDir + "/"+m_dataset+"/annotations/set01/V000.vbb").c_str());
    annotationReader.setVerbose(0);
    annotationReader.readVdd();
    
    ImageExtractor extractor;
    int m_doResizePersonsX = 96; 
    int m_doResizePersonsY = 160;

    extractor.m_resize = true;
    extractor.m_resizeX = m_doResizePersonsX;
    extractor.m_resizeY = m_doResizePersonsY;

    extractor.setExtractionPath((outDir + "/"+m_dataset+"/extraction/set01/V000").c_str());


    BufferedImage image(header.width, header.height);

    int i = 602;
    
    printf("Read image %d\n", i);
    
    extractor.setFrameNumber(i);
    
    for (int k=0; k < i; k++)
    {
        reader.skipImageData(&header);
        printf("skiping frame %d\n", k);
    }
    
    reader.readImageData(&header, &image);

    image.convertToMonochrome();
                
    extractor.setImage(&image);
    extractor.extractFrame();
    
    printf("Frame extracted\n");
    
    std::vector<BoundingBox> annotatedBoxes = annotationReader.getBoundingBoxes(i);
    std::vector<BoundingBox> centeredBoxes = annotatedBoxes;

    //    if (m_dumpAnnotationInfo)
    {
        for (int b=0; b < annotatedBoxes.size(); b++)
        {
            BoundingBox box = annotatedBoxes.at(b);

            printf("Box:  pos (%d,%d)  dim (%d,%d) Id %d Occluded %d V: pos (%f,%f)  dim (%f,%f)\n", 
                    (int) box.m_x, (int)box.m_y, (int) box.m_width, (int) box.m_height,
                    box.m_objId, box.m_occluded,
                    box.m_vx, box.m_vy, box.m_vw, box.m_vh);
        }
    }
    

    for (int b=0; b < centeredBoxes.size(); b++)
    {
        BoundingBox box = centeredBoxes.at(b);

//        if (box.m_width >= m_minPersonWidth)
        {
            // Avoid centering (expanding the width) of persons lower than 
            // the threshold so that they can be later filtered out
            box.centerBox(m_doResizePersonsX, m_doResizePersonsY);
            centeredBoxes.at(b) = box;
        }
    }

//    if (m_dumpAnnotationInfo)
    {
        for (int b=0; b < centeredBoxes.size(); b++)
        {
            BoundingBox box = centeredBoxes.at(b);

            printf("Centered Box:  pos (%d,%d)  dim (%d,%d) Id %d Occluded %d V: pos (%f,%f)  dim (%f,%f)\n", 
                    (int) box.m_x, (int)box.m_y, (int) box.m_width, (int) box.m_height,
                    box.m_objId, box.m_occluded,
                    box.m_vx, box.m_vy, box.m_vw, box.m_vh);
        }
    }          
    
    std::vector<Image*> persons = extractor.getPersonsHigher(centeredBoxes, 10, 10);
    std::vector<Image*> inputPersons = extractor.resizePersons(persons);

    for (int i=0; i < inputPersons.size(); i++)
    {
        
        extractor.saveSvmTraining(inputPersons[i], true);
    
        printf("Subimage %d saved\n", i);
    }
    return 0;
}

