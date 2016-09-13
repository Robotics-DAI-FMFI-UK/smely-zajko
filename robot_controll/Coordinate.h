#ifndef COORDINATE_H
#define COORDINATE_H

#include <cstdlib>
#include <cv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#include "EvalDireciton.h"
#include "SbotThread.h"

extern bool autonomy;

class Coordinate {
  public:
    int wrong_dir;
    double running_mean;
    double predicted_dir;
    double computed_dir;
    string move_status;
    bool status_from_subroutines;
    Coordinate();

    int move(CvMat* predicted_data, SbotThread* sbot, double mapAngle,
             double imuAngle, EvalDireciton* ed, int* laserData);

  private:
};

#endif /* COORDINATE_H */
