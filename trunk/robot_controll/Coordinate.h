/* 
 * File:   Coordinate.h
 * Author: myron
 *
 * Created on Piatok, 2011, máj 6, 12:10
 */

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
    Coordinate();

    int move(CvMat* predicted_data, SbotThread* sbot, double mapAngle, double imuAngle, EvalDireciton* ed );

private:

};

#endif	/* COORDINATE_H */
