#ifndef COORDINATE_H
#define COORDINATE_H

#include <cstdlib>
#include <cv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#include "EvalDirection.h"
#include "SbotThread.h"
#include "DataTypes.h"

extern bool autonomy;

class Coordinate {
  public:
    int wrong_dir;
    double running_mean;
    double predicted_dir;
    double computed_dir;
    double running_mean_weight;
    double speed_down_dst;
    string move_status;
    bool status_from_subroutines;
    Coordinate();

    int move(CvMat* predicted_data, SbotThread* sbot, GpsAngles angles,
                     double imuAngle, EvalDirection* ed, int* laserData);

  private:
};

#endif /* COORDINATE_H */
