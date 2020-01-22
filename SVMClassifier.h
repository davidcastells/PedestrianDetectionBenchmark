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
 * File:   SVMClassifier.h
 * Author: dcr
 *
 * Created on February 19, 2018, 6:45 PM
 */

#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include "HOGFeature.h"

#include <stdio.h>
#include <svm.h>

#include <string>
#include <vector>

class SVMClassifier {
public:
    SVMClassifier();
    
    virtual ~SVMClassifier();
public:
    void setTrainingInputFile(std::string& inputFile);
    void setModelFile(std::string& modelFile);
    void readProblem();
    void trainModel();
    void importModel();
    void exportModel();
    void createSvmFile();
    void savePersonHogSvms(std::vector<HOGFeature*> persons);
    void saveNonPersonHogSvms(std::vector<HOGFeature*> nonPersons);
    double predict(HOGFeature* feature);
    void appendHogFeatureToSvmFile(bool isPerson, HOGFeature* feature);
    int getNumSupportVectors();
    void configureFeatureDimension(int m_doResizePersonsX, int m_doResizePersonsY, int m_doMonochrome);
    
    // Copied from libsvm
protected:
    double my_svm_predict_values(const svm_model *model, double *x, double* dec_values);
    double my_svm_predict_probability(const svm_model *model, double *x, double *prob_estimates);
    double dotProduct(double* x);
    
private:
    
    char* readLine(FILE *input);
    void createSvmNodeFromHogFeature(HOGFeature* feature, svm_node* svmVector) __attribute__((deprecated));
    void createArrayFromHogFeature(HOGFeature* feature, double* featureVector);

private:
    struct svm_parameter param;		// set by parse_command_line
    struct svm_problem prob;		// set by read_problem
    struct svm_node *x_space;
    struct svm_model *model;
    
//    double* m_svm_kvalue;
    double* m_svm_w;
    int m_svm_size;
    
    double* predict_x;
    //svm_node* predict_x;
    int predict_x_size;
    
    char* line;
    int max_line_len;
    
    std::string m_trainingInputFile;
    std::string m_modelFile;
    
public:
    int m_positiveSample;
    int m_negativeSample;
};

#endif /* SVMCLASSIFIER_H */

