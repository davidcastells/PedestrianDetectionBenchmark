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
#include <assert.h>
#include <math.h>

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
    
    predict_x = NULL;
    predict_x_size = 0;
    
//    m_svm_kvalue = NULL;
}



SVMClassifier::~SVMClassifier()
{
    if (model != NULL)
        svm_free_and_destroy_model(&model);
    
//    if (m_svm_kvalue) delete m_svm_kvalue;
    
    //svm_destroy_param(&param);
    if (prob.y) free(prob.y);
    if (prob.x) free(prob.x);
    if (x_space) free(x_space);
    if (line) free(line);
    if (predict_x) delete predict_x;
}

int SVMClassifier::getNumSupportVectors()
{
    return model->l;
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

static double my_sigmoid_predict(double decision_value, double A, double B)
{
	double fApB = decision_value*A+B;
	// 1-p used later; avoid catastrophic cancellation
	if (fApB >= 0)
		return exp(-fApB)/(1.0+exp(-fApB));
	else
		return 1.0/(1+exp(fApB)) ;
}

#ifndef min
template <class T> static inline T min(T x,T y) { return (x<y)?x:y; }
#endif
#ifndef max
template <class T> static inline T max(T x,T y) { return (x>y)?x:y; }
#endif

double my_dot(const svm_node *px, const svm_node *py)
{
	double sum = 0;
	while(px->index != -1 && py->index != -1)
	{
            if(px->index == py->index)
            {
                sum += px->value * py->value;
                ++px;
                ++py;
            }
            else
            {
                if(px->index > py->index)
                    ++py;
                else
                    ++px;
            }
	}
	return sum;
}

double my_k_function(const svm_node *x, const svm_node *y)
{
    return my_dot(x,y);
}

double SVMClassifier::dotProduct(double* x)
{
    assert(predict_x_size == m_svm_size);
    
    double sum = 0;
    
    for (int i=0; i < predict_x_size; i++)
    {
        sum += predict_x[i] * m_svm_w[i];
    }
    
    return sum;
}

double SVMClassifier::my_svm_predict_values(const svm_model *model, double *x, double* dec_values)
{
	int i;
	assert(!(model->param.svm_type == ONE_CLASS ||
	   model->param.svm_type == EPSILON_SVR ||
	   model->param.svm_type == NU_SVR));


		int nr_class = model->nr_class;
                assert(nr_class == 2);
		int l = model->l;

		//double *kvalue = Malloc(double,l);
//		for (i=0;i<l;i++)
//			m_svm_kvalue[i] = my_k_function(x, model->SV[i]);

		int start[2]; //  = Malloc(int,nr_class);
		start[0] = 0;
		start[1] = model->nSV[0];

		int vote[2];
		for(i=0;i<2;i++)
			vote[i] = 0;

		int p=0;
		i=0;
                int j = 1;
                
//                for(i=0;i<2;i++)
//			for(int j=i+1;j<2;j++)
			{
				double sum = 0;
				int si = start[i];
				int sj = start[j];
				int ci = model->nSV[i];
				int cj = model->nSV[j];

				int k;
				double *coef1 = model->sv_coef[j-1];
				double *coef2 = model->sv_coef[i];
				
//                                for(k=0;k<ci;k++)
//					sum += coef1[si+k] * m_svm_kvalue[si+k];
//                                
//				for(k=0;k<cj;k++)
//					sum += coef2[sj+k] * m_svm_kvalue[sj+k];
                                
                                sum = dotProduct(x);
				sum -= model->rho[p];
				dec_values[p] = sum;

				if(dec_values[p] > 0)
					++vote[i];
				else
					++vote[j];
				p++;
			}

		int vote_max_idx = 0;
		for(i=1;i<nr_class;i++)
			if(vote[i] > vote[vote_max_idx])
				vote_max_idx = i;

		//free(kvalue);
		//free(start);
		//free(vote);
		return model->label[vote_max_idx];
	
}

double SVMClassifier::my_svm_predict_probability(const svm_model *model, double  *x, double *prob_estimates)
{
	assert ((model->param.svm_type == C_SVC || model->param.svm_type == NU_SVC) &&
	    model->probA!=NULL && model->probB!=NULL);
	
		int i;
		int nr_class = model->nr_class;
                assert(nr_class == 2);
                
		double dec_values[2]; // = Malloc(double, nr_class*(nr_class-1)/2);
		my_svm_predict_values(model, x, dec_values);

		double min_prob=1e-7;
                double pairwise_prob[2][2];
//		double **pairwise_prob=Malloc(double *,nr_class);
//		for(i=0;i<nr_class;i++)
//			pairwise_prob[i]=Malloc(double,nr_class);
		int k=0;
		for(i=0;i<nr_class;i++)
			for(int j=i+1;j<nr_class;j++)
			{
				pairwise_prob[i][j]=min(max(my_sigmoid_predict(dec_values[k],model->probA[k],model->probB[k]),min_prob),1-min_prob);
				pairwise_prob[j][i]=1-pairwise_prob[i][j];
				k++;
			}
		if (nr_class == 2)
		{
			prob_estimates[0] = pairwise_prob[0][1];
			prob_estimates[1] = pairwise_prob[1][0];
		}
		else
                    assert(false);//			multiclass_probability(nr_class,pairwise_prob,prob_estimates);

		int prob_max_idx = 0;
		for(i=1;i<nr_class;i++)
			if(prob_estimates[i] > prob_estimates[prob_max_idx])
				prob_max_idx = i;
//		for(i=0;i<nr_class;i++)
//			free(pairwise_prob[i]);
//		free(dec_values);
//		free(pairwise_prob);
		return model->label[prob_max_idx];
	
}

double SVMClassifier::predict(HOGFeature* feature)
{
    double ret;
    double p[3] = {-1, -1};     // person , non-person
    //struct svm_node *x = createSvmNodeFromHogFeature(feature);
    
    if (predict_x == NULL)
    {
        predict_x_size = feature->getTotalBins();
        predict_x = new double[predict_x_size];
    }
    else if (predict_x_size < (feature->getTotalBins()+1))
    {
        delete [] predict_x;
        predict_x_size = feature->getTotalBins();
        predict_x = new double[predict_x_size];
    }
    
    //int sx = sizeof(svm_node) * predict_x_size;
    
    
    //memset(x, 0, sx);
    createArrayFromHogFeature(feature, predict_x);
    
    //ret = svm_predict(model, x);
    ret = my_svm_predict_probability(model, predict_x, p);
    
    //free(x);
    return p[0];
}

void SVMClassifier::createArrayFromHogFeature(HOGFeature* feature, double* featureVector)
{
    int featureNum = 0;
    
    for (int c=0; c < feature->m_colorChannels; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        double* pBin = feature->getBin(bx, by, x, y, c);

                        //fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);
                        
                        for (int i=0; i < 9; i++)
                        {
                            featureVector[featureNum] = pBin[i];
                            featureNum++;
                        }
                    }
}

/**
 * 
 * @param feature
 * @param svmVector
 * @deprecated use the vector one
 */
void SVMClassifier::createSvmNodeFromHogFeature(HOGFeature* feature, svm_node* svmVector)
{
    //struct svm_node* svmVector = (struct svm_node *) malloc(feature->getTotalBins() * sizeof(struct svm_node));
    
    int featureNum = 1;

    
    for (int c=0; c < feature->m_colorChannels; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        double* pBin = feature->getBin(bx, by, x, y, c);

                        //fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);
                        
                        for (int i=0; i < 9; i++)
                        {
                            svmVector[featureNum].index = featureNum;
                            svmVector[featureNum].value = pBin[i];
                            featureNum++;
                        }
                    }
    
    svmVector[featureNum].index = -1;
//    return svmVector;
}

/**
 * inspired from https://github.com/cjlin1/libsvm/blob/master/svm-predict.c
 */
void SVMClassifier::importModel()
{
    model = svm_load_model(m_modelFile.c_str());
    
    int l = model->l;   // number of support vectors
    
    
//    m_svm_kvalue = new double[l];
    
//    m_svm_w = model->
    
//    printf("load model = %p\n", model);
}


double svm_node_get_index(svm_node* node, int index)
{
    for (int i=0; 1; i++)
    {
        if (node[i].index == index)
            return node[i].value;
        if (node[i].index == -1)
            return 0;
    }
    
    return 0;
}

void SVMClassifier::configureFeatureDimension(int m_doResizePersonsX, int m_doResizePersonsY, int m_doMonochrome)
{
    int m_numBlocksX = 2 * ((m_doResizePersonsX / 8) / 2) - 1;
    int m_numBlocksY = 2 * ((m_doResizePersonsY / 8) / 2) - 1;
    int m_blockWidth = 2;
    int m_blockHeight= 2;
    int m_colorChannels = (m_doMonochrome)? 1 : 3;
    
    int totalBins = m_numBlocksX * m_numBlocksY * m_blockWidth * m_blockHeight * 9 * m_colorChannels;
    m_svm_w = new double[totalBins];
    m_svm_size = totalBins;
    
    int nr_class = model->nr_class;
    assert(nr_class == 2);
    int l = model->l;
                
    int start[2]; //  = Malloc(int,nr_class);
    start[0] = 0;
    start[1] = model->nSV[0];
    
    int percent = 0;
    
    for (int i = 0; i < totalBins; i++)
    {
        if (((i * 100) / totalBins) != percent)
        {
            percent = (i * 100) / totalBins;
            printf("Precomputing coefficient for feature bin %d%%\n", percent);
        }
        
        m_svm_w[i] = 0;
        
        // Class 0 Support Vector
        for (int k=0; k < l; k++)
        {
            m_svm_w[i] += svm_node_get_index(model->SV[k], i+1) * model->sv_coef[0][k];
        }
        
        
    }
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
    
    fprintf(fp, (isPerson)? "1 " : "-1 ");  // This is the label (1) means positive sample(person) , (0) means negative (non-person)
    
    if (isPerson)
        m_positiveSample++;
    else
        m_negativeSample++;
    
    int featureNum = 1;
    
    for (int c=0; c < feature->m_colorChannels; c++)
        for (int by = 0; by < feature->m_numBlocksY; by++)
            for (int bx = 0; bx < feature->m_numBlocksX; bx++)
                for (int y=0; y < feature->m_blockHeight; y++)
                    for (int x=0; x < feature->m_blockWidth; x++)
                    {
                        double* pBin = feature->getBin(bx, by, x, y, c);

                        //fprintf(fp, "%d; %d; %d; %d; %d; ", c, bx, by, x, y);

                        for (int i=0; i < 9; i++)
                            fprintf(fp, "%d:%f ", featureNum++, pBin[i]);

                        
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