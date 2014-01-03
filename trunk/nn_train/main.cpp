/* 
 * File:   main.cpp
 * Author: Myron
 *
 * Created on Piatok, 2009, december 4, 22:29
 */

#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include <vector>

#include <time.h>

#include <cv.h>
#include <ml.h>
#include <highgui.h>

#include <dirent.h>
#include <locale.h>

#include <string>

using namespace std;

#include "VisionBase.h"
#include "VisionRegionReduced.h"
#include "VisionRegion.h"
#include "VisionHistogram.h"


FILE* train_log;



void create_train_data(vector<string> files, string dir, VisionBase* nn, time_t start_time){

    vector<IplImage*> inputs;
    vector<CvMat*> outputs;

    //load files
    printf("loading files...\n" );

    for(int i=0; i<files.size(); i++){

//printf("%s \n", files.at(i).c_str() );
        if( files.at(i).length()>4 ){

            string suffix = files.at(i).substr( files.at(i).length()-4, 4 );

            if( suffix.compare(".jpg")==0 || suffix.compare(".JPG")==0 || suffix.compare(".png")==0 || suffix.compare(".PNG")==0 ){ // jpg, png ....
//printf("%s %s \n", suffix.c_str(), files.at(i).c_str() );
                string filepath(dir);
                filepath += files.at(i);

                IplImage* tmp = cvLoadImage( (const char *)filepath.c_str() );
                inputs.push_back( cvCreateImage( cvSize( nn->image_width, nn->image_height), tmp->depth, tmp->nChannels) );
                cvResize( tmp, inputs.back() ); //resize to previously inserted image
                cvReleaseImage( &tmp );

                //outputs
                string xml_path = filepath.substr( 0, filepath.length()-4 );
                xml_path += ".xml";

//printf("%s \n", xml_path.c_str() );
                CvMat* out =  (CvMat*)cvLoad( (const char *)xml_path.c_str() );
                if(out==NULL){
                    printf("error cannot read xml file : %s\n", xml_path.c_str() );
                    return;
                }

                outputs.push_back( cvCreateMat(nn->image_height, nn->image_width, CV_8U )  );
                cvResize( out, outputs.back() ); //resize to previously inserted matrix
                cvReleaseMat( &out );
            }
        }
    }

    //prepare data
    printf("preparing data...\n" );

    int sample_count = 500;

    //create files with train samples
    char filename[64];
    sprintf(filename, "data%d.data", start_time);
    nn->create_random_training( inputs, outputs, sample_count, filename );

    printf("inputs count: %d\n",inputs.size()*sample_count );
    fprintf(train_log, "inputs count: %d\n",inputs.size()*sample_count );

    for(int i=0; i<inputs.size(); i++){
        cvReleaseImage( &inputs[i] );
        cvReleaseMat( &outputs[i] );
    }

    inputs.clear();
    outputs.clear();
}

/*
 *  usage: nn_train <dir to training data>
 */
int main(int argc, char** argv) {

    setlocale(LC_ALL, "C");
    srand( time(NULL) );

    //log
    time_t start_time;
    time(&start_time);

    char logname[64];
    sprintf(logname, "../train_logs/%d.log", start_time);
    train_log = fopen( logname, "w");
    if( train_log == NULL ){
        printf("log cannot be created.\n");
        exit(-1);
    }


    int input_size = 6;
    if(argc>2){
        input_size = atoi( argv[2] );
    }

    int pattern_size = 6;
    if(argc>3){
        pattern_size = atoi( argv[3] );
    }

    int hidden_neurons = 10;
    if(argc>4){
        hidden_neurons = atoi( argv[4] );
    }

    int hidden_layers = 1;
    if(argc>5){
        hidden_layers = atoi( argv[5] );
    }

    int type = 1;
    if(argc>6){
        type = atoi( argv[6] );
    }

    VisionBase* nn;

    if(type==1){
        nn = new VisionBase(720, 576,input_size, input_size);
    }else if(type==2){
        nn = new VisionRegionReduced(720, 576,input_size, input_size, pattern_size); //sqdiff
    }else if(type==3){
        nn = new VisionRegion(720, 576,input_size, input_size, pattern_size);// raw
    }else if(type==4){
        nn = new VisionHistogram(720, 576,input_size, input_size, pattern_size); //with histogram
    }

    //init
    nn->init( hidden_neurons, hidden_layers );

    if(argc>1){

        //read dir
        string dir(argv[1]);
        if( dir.substr( dir.length()-1, 1 ).compare("/")!=0 ){
            dir += "/";
        }

        vector<string> files;

        DIR *dp;
        struct dirent *dirp;
        if((dp  = opendir( dir.c_str() )) == NULL) {
            printf("error cannot open directory\n");
            return -2;
        }

        while ((dirp = readdir(dp)) != NULL) {
    //printf("%s \n", dirp->d_name );
            files.push_back(string(dirp->d_name));
        }
        closedir(dp);

        create_train_data(files, dir, nn, start_time);
        files.clear();
    }else{
        printf("usage: nn_train <dir to training data> <input_size> <pattern_size/hist_size> <neuron_count>\n");
        exit(-1);
    }

    //TODO: we don't need to scale each time, but we need scale constants to be set to ann structure
    char dataFile[64];
    sprintf(dataFile, "data%d.data", start_time);
    char dataNormFile[64];
    sprintf(dataNormFile, "dataNorm%d.data", start_time);
    char trainNormFile[64];
    sprintf(trainNormFile, "trainNorm%d.data", start_time);
    char testNormFile[64];
    sprintf(testNormFile, "testNorm%d.data", start_time);
    char valNormFile[64];
    sprintf(valNormFile, "valNorm%d.data", start_time);

    nn->scale_data(dataFile, dataNormFile );
    nn->divide_data(dataNormFile, trainNormFile, testNormFile, valNormFile );

    
    fprintf(train_log, "image_width: %d\n", nn->image_width);
    fprintf(train_log, "image_height: %d\n", nn->image_height);

    fprintf(train_log, "step_x: %d\n", nn->step_x);
    fprintf(train_log, "step_y: %d\n", nn->step_y);

    //fprintf(train_log, "pattern size: %d\n", nn.pattern_size);

    //training
    printf("training...\n" );

    char net_name[64];
    sprintf(net_name, "../train_logs/%d.net", start_time);

    int max_epochs = 10;

    time_t train_start = time(NULL);

    double test_error = DBL_MAX;
    double prev_test_error = DBL_MAX;
    double best_test_error = DBL_MAX;

        
    fprintf(train_log, "number of hidden neurons: %d\n", hidden_neurons );

    double val_error = DBL_MAX;
    double prev_val_error = DBL_MAX;
    double best_val_error = DBL_MAX;
    int epoch = 0;

    while( val_error <= prev_val_error ){


        printf("epoch: %d\n", epoch*max_epochs );
        fprintf(train_log,"epoch: %d\n", epoch*max_epochs );

        double train_error = nn->train( 0,  max_epochs, trainNormFile );
        fprintf(train_log, "train_error: %f\n", train_error );

        prev_val_error = val_error;
        val_error = nn->test( valNormFile );

        printf("validation error: %f\n", val_error);
        fprintf(train_log, "validation error: %f\n", val_error);

        epoch ++;
    }

    //test
    prev_test_error = test_error;

    test_error = nn->test(testNormFile);

    printf("test error: %f\n", test_error);
    fprintf(train_log, "test error: %f\n", test_error);

    if( test_error < best_test_error ){
        best_test_error = test_error;
        nn->save( net_name ); //rewrite better
        printf("network saved\n");
        fprintf(train_log, "network saved\n");
    }
    nn->close(); //close previous

    
    time_t train_end = time(NULL);
    
    printf("> train end %d \n", train_end - train_start);
    fprintf(train_log, "time spent: %d\n", train_end - train_start);
    fprintf(train_log, "best test error: %f\n", best_test_error);
    
    fclose( train_log );

    remove( dataFile );
    remove( dataNormFile );
    remove( trainNormFile );
    remove( testNormFile );
    remove( valNormFile );

    return (EXIT_SUCCESS);
}

