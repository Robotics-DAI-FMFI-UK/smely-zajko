#ifndef SENSORMANAGEMENT_H
#define SENSORMANAGEMENT_H

#include <cstdlib>
#include <cv.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include "BindSerialPorts.h"
#include "ContextHistogram.h"
#include "ContextProbableRectangle.h"
#include "Coordinate.h"
#include "EvalDirection.h"
#include "GpsThread.h"
#include "HokuyoThread.h"
#include "ImuThread.h"
#include "ModKMeans.h"
#include "SbotThread.h"
#include "VisionBaseNew.h"

class SensorManagement {
  public:
    Coordinate* coor;
    GpsThread* gps;
    ImuThread* imu;
    SbotThread* sbot;
    HokuyoThread* hokuyo;
    LocalizationAndPlaning* loc;
    VisionBase* nn;
    EvalDirection* ed;

    SbotData sdata;
    ImuData idata;
    Ll gdata;
    GpsAngles angles; // volalo sa "a" predtym
    CvMat* predicted_data;
    int* laser;

    int online_mode;
    
    SensorManagement();

    int move();
    void init(int is_online_mode);
    int update();
    int getDirNum();
    double evalNeuralForDirection(int dir);
    IplImage* getLaserFrame();
    void stop();
    int neuralPredict(IplImage* frame);
    void saveHokuyoLog(char* path);

  private:
};

#endif /* SENSORMANAGEMENT_H */
