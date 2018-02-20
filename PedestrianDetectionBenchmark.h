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
 * File:   PedestrianDetectionBenchmark.h
 * Author: dcr
 *
 * We tried to find an OpenSource HOG-based pedestrian detection software
 * and we didn't find any written in C/C++, so we decided to write one.
 * 
 * We use the caltech pedestrian detection benchmark data to test our system.
 * 
 * Created on February 1, 2018, 11:04 AM
 */

#ifndef PEDESTRIANDETECTIONBENCHMARK_H
#define PEDESTRIANDETECTIONBENCHMARK_H

#include <string>

class PedestrianDetectionBenchmark {
public:
    PedestrianDetectionBenchmark();
    virtual ~PedestrianDetectionBenchmark();
private:

public:
    void parseOptions(int argc, char* args[]);
    void validateOptions();
    
    void run();
    void usage();
    int getAnnotatedFrames();
    
protected:
    bool m_doUsage;
    bool m_doPlayInputSequence;
    bool m_doPlayInputAsHog;
    bool m_doExtractFrames;
    bool m_doExtractAnnotatedPersons;
    bool m_doExtractHogPersons;
    bool m_doExtractHogFeatures;
    bool m_doExtractHogSvm;
    bool m_doTrainSvmFromFiles;
    bool m_doPredict;
    bool m_doMaxFps;
    bool m_doYuv;
    bool m_doMonochrome;
    bool m_doRotateHog;
    bool m_doResizePersons;
    int m_minPersonHeight;
    int m_zoom;
    int m_startInFrame;
    int m_doResizePersonsX;
    int m_doResizePersonsY;
    double m_fps;
    std::string m_dataset;
};

#endif /* PEDESTRIANDETECTIONBENCHMARK_H */

