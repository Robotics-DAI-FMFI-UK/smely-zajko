/* 
 * File:   VisionBase.h
 * Author: mnam
 *
 * Created on April 27, 2014, 9:01 PM
 */

#ifndef NEURAL_NETWORK_H_INCLUDED
#define NEURAL_NETWORK_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <fann.h>

#include "VisionContext.h"
#include "VisionModifier.h"

using namespace std;

class VisionBase{

public:
    int n_count;

    int image_width,image_height;
    int parts_x,parts_y;
    int step_x, step_y;
    int in_x, in_y;
    int out_x, out_y;
    int channels;
    int out_width,out_height;

    VisionContext* context;
    VisionModifier* modifier;

    
    int input_neurons;
    int output_neurons;

    VisionBase(int step_x, int step_y, int in_x, int in_y, int out_x, int out_y, VisionContext* con,vector<int> vcp, VisionModifier* mod,vector<int> vmp);
    void init(int hidden_neurons, int hidden_layers);
    void load(const char* filepath);
    void save(const char* filepath);


    double train( const float desired_error, const unsigned int max_epochs, char* train_file );
    double train_on_data( const float desired_error, const unsigned int max_epochs, fann_train_data* data );

    double test(char* train_file);
    double test_on_data(fann_train_data* data);
    
    CvMat* predict(IplImage* inputs);
    
    void close();

    virtual int create_training_file_full( vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file );
    virtual int create_training_file_partial( vector<IplImage*> inputs, vector< vector<int> >& outputs,  char* out_file, int samples );
    virtual  vector<int>  upscale_result(vector<fann_type> v,int image_width, int image_height);
    void scale_data(char* file, char* out_file);
    
    struct fann *ann;
    
protected :
    virtual CvMat* create_out_mat(vector<fann_type> calc_out);


};

#endif	/* NEURAL_NETWORK_H_INCLUDED */

