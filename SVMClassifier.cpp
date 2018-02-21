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
 * File:   SVMClassifier.cpp
 * Author: dcr
 * 
 * Created on February 19, 2018, 6:45 PM
 */

#include "SVMClassifier.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <string>

SVMClassifier::SVMClassifier()
{
    param.svm_type = C_SVC; // ONE_CLASS;
    param.kernel_type = LINEAR;
    param.degree = 1;
    param.gamma = 0;	// 1/num_features
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 100;
    param.C = 1;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 1;
    param.probability = 1; // 0;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;
    
    m_positiveSample = 0;
    m_negativeSample = 0;
    
    model = NULL;
    prob.y = NULL;
    prob.x = NULL;
    x_space = NULL;
    line = NULL;
}

SVMClassifier::SVMClassifier(const SVMClassifier& orig)
{
    
}

SVMClassifier::~SVMClassifier()
{
    if (model != NULL)
        svm_free_and_destroy_model(&model);
    
    //svm_destroy_param(&param);
    if (prob.y) free(prob.y);
    if (prob.x) free(prob.x);
    if (x_space) free(x_space);
    if (line) free(line);
}

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

/**
 * From https://github.com/niosus/SVM_example/blob/master/svm.cpp
 * @param input
 * @return 
 */
char* SVMClassifier::readLine(FILE *input)
{
    int len;

    if(fgets(line,max_line_len,input) == NULL)
        return NULL;

    while(strrchr(line,'\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *) realloc(line,max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
            break;
    }
    return line;
}

void SVMClassifier::trainModel()
{
    model = svm_train(&prob,&param);
}

void SVMClassifier::exportModel()
{
    svm_save_model(m_modelFile.c_str(), model);
}

double SVMClassifier::predict(HOGFeature* feature)
{
    double ret;
    double p[3] = {-1, -1};     // person , non-person
    struct svm_node *x = createSvmNodeFromHogFeature(feature);
    
    //ret = svm_predict(model, x);
    ret = svm_predict_probability(model, x, p);
    
    //free(x);
    return p[0];
}


svm_node* SVMClassifier::createSvmNodeFromHogFeature(HOGFeature* feature)
{
    struct svm_node* svmVector = (struct svm_node *) malloc(feature->getTotalBins() * sizeof(struct svm_node));
    
    int featureNum = 1;

    
    for (int c=0; c < 3; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        unsigned int* pBin = feature->getBin(bx, by, x, y, c);

                        //fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);
                        
                        for (int i=0; i < 9; i++)
                        {
                            svmVector[featureNum].index = featureNum;
                            svmVector[featureNum].value = pBin[i];
                            featureNum++;
                        }
                    }
    
    return svmVector;
}

/**
 * inspired from https://github.com/cjlin1/libsvm/blob/master/svm-predict.c
 */
void SVMClassifier::importModel()
{
    model = svm_load_model(m_modelFile.c_str());
}

void SVMClassifier::setTrainingInputFile(std::string& inputFile)
{
    m_trainingInputFile = inputFile;
}

void SVMClassifier::setModelFile(std::string& modelFile)
{
    m_modelFile = modelFile;
}


/**
 * Reads a problem file from disk and fills the related structures
 * (prob)
 * From https://github.com/cjlin1/libsvm/blob/master/svm-train.c
 * @param filename
 */
void SVMClassifier::readProblem()
{
    const char* filename = m_trainingInputFile.c_str();
    
    int max_index, inst_max_index, i;
    size_t elements, j;
    FILE *fp = fopen(filename,"r");
    char *endptr;
    char *idx, *val, *label;

    if(fp == NULL)
    {
        fprintf(stderr,"can't open input file %s\n",filename);
        exit(1);
    }

    prob.l = 0;
    elements = 0;

    max_line_len = 1024;
    line = Malloc(char, max_line_len);
    
    while(readLine(fp)!=NULL)
    {
        char *p = strtok(line," \t"); // label

        // features
        while(1)
        {
            p = strtok(NULL," \t");
            if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
                break;
            ++elements;
        }
        ++elements;
        ++prob.l;
    }
    rewind(fp);

    prob.y = Malloc(double,prob.l);
    prob.x = Malloc(struct svm_node *,prob.l);
    x_space = Malloc(struct svm_node,elements);

    max_index = 0;
    j=0;
    for(i=0;i<prob.l;i++)
    {
        inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
        readLine(fp);
        prob.x[i] = &x_space[j];
        label = strtok(line," \t\n");
        
        if(label == NULL) // empty line
        {
            printf("[ERROR]\n");
            exit(-1);
            //exit_input_error(i+1);
        }

        prob.y[i] = strtod(label,&endptr);
        if(endptr == label || *endptr != '\0')
        {
            printf("[ERROR]\n");
            exit(-1);
            //exit_input_error(i+1);
        }

        while(1)
        {
                idx = strtok(NULL,":");
                val = strtok(NULL," \t");

                if(val == NULL)
                    break;

                errno = 0;
                x_space[j].index = (int) strtol(idx,&endptr,10);
                if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
                {
                    printf("[ERROR]\n");
                    exit(-1);
                    //exit_input_error(i+1);
                }
                else
                    inst_max_index = x_space[j].index;

                errno = 0;
                x_space[j].value = strtod(val,&endptr);
                if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
                {
                    printf("[ERROR]\n");
                    exit(-1);
                    //exit_input_error(i+1);
                }

                ++j;
        }

        if(inst_max_index > max_index)
            max_index = inst_max_index;
        x_space[j++].index = -1;
    }

    if(param.gamma == 0 && max_index > 0)
        param.gamma = 1.0/max_index;

    if(param.kernel_type == PRECOMPUTED)
        for(i=0;i<prob.l;i++)
        {
            if (prob.x[i][0].index != 0)
            {
                fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
                exit(1);
            }
            if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
            {
                fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
                exit(1);
            }
        }

    fclose(fp);
}

void SVMClassifier::appendHogFeatureToSvmFile(bool isPerson, HOGFeature* feature)
{
    const char* filename = m_trainingInputFile.c_str();
    
    FILE* fp = fopen(filename, "a+");
    
    fprintf(fp, (isPerson)? "1 " : "0 ");  // This is the label (1) means positive sample(person) , (0) means negative (non-person)
    
    if (isPerson)
        m_positiveSample++;
    else
        m_negativeSample++;
    
    int featureNum = 1;
    
    for (int c=0; c < 3; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        unsigned int* pBin = feature->getBin(bx, by, x, y, c);

                        //fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);

                        for (int i=0; i < 9; i++)
                            fprintf(fp, "%d:%d ", featureNum++, pBin[i]);

                        
                    }
    
    fprintf(fp, "\n");
    
    fclose(fp);
}

void SVMClassifier::savePersonHogSvms(std::vector<HOGFeature*> persons)
{
    for (int i=0; i < persons.size(); i++)
    {
        HOGFeature* feature = persons.at(i);
        
        printf("adding pos feature to SVM\n");
        appendHogFeatureToSvmFile(true, feature);        
    }
}

void SVMClassifier::saveNonPersonHogSvms(std::vector<HOGFeature*> nonPersons)
{
    for (int i=0; i < nonPersons.size(); i++)
    {
        HOGFeature* feature = nonPersons.at(i);
        
        printf("adding neg feature to SVM\n");
        appendHogFeatureToSvmFile(false, feature);        
    }
}

void SVMClassifier::createSvmFile()
{    
    FILE* fp = fopen(m_trainingInputFile.c_str(), "w");
    fclose(fp);
}