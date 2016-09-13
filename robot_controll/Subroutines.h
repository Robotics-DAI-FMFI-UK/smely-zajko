#ifndef SUBROUTINES_H
#define SUBROUTINES_H

#include <cstdlib>
#include <cv.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

#include "SbotThread.h"
#include "SensorManagement.h"

extern bool autonomy;

class Subroutines {
  public:
    SbotThread* sbot;
    LocalizationAndPlaning* loc;

    Ll startpos;
    int setstart;
    int finishcount;
    long found_obstacle;
    int finnumber;
    bool was_obstacle;

    Subroutines();

    void testSbot();
    void setup(LocalizationAndPlaning* lc, SbotThread* sb);
    int manageFinish(SensorManagement sm);
    void manageObstacles(SensorManagement sm);

  private:
};

#endif /* SUBROUTINES_H */
