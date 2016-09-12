#ifndef COORDINATE_H
#define	COORDINATE_H

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <vector>

using namespace std;

#include "SbotThread.h"
#include "EvalDireciton.h"

extern bool autonomy;

class Coordinate {
public:
	int wrong_dir;
    double running_mean;
    double predicted_dir;
    double computed_dir;
    Coordinate();

    int move(CvMat* predicted_data, SbotThread* sbot, double mapAngle, double imuAngle, EvalDireciton* ed, int* laserData  );

private:

};

#endif	/* COORDINATE_H */

