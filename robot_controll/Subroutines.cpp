#include "Subroutines.h"

Subroutines::Subroutines() {
    was_obstacle = false;
    setstart = 0;
    finishcount = 0;
    found_obstacle = 0;
    finnumber = 0;
}

void Subroutines::testSbot() {
    sleep(1);
    sbot->setSpeed(1);
    sleep(1);
    sbot->setSpeed(0);
}

void Subroutines::setup(LocalizationAndPlaning* lc, SbotThread* sb) {
    loc = lc;
    sbot = sb;
}

void say(string);

void Subroutines::manageObstacles(SensorManagement sm) {
    int hokuyo_to_stop = HokuyoThread::demands_immediate_stop();
    int see_obstacle = sm.sdata.obstacle || hokuyo_to_stop;
    if (hokuyo_to_stop) 
    {
        sbot->stopNow();
        say("I beg your pardon");
    }

    if (see_obstacle && (!was_obstacle)) {
        time_t tobs = time(NULL);
        found_obstacle = tobs;
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
        sm.coor->move_status = "obstacle";
    }
    time_t tmnow = time(NULL);
    if (see_obstacle && (tmnow - found_obstacle > 20)) {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
        found_obstacle = found_obstacle + 30;
        // cuvat alebo aspon tocit!!
        int back_direction = 0;
        // if compass has smaller delta direction
        if(fabs(sm.coor->delta) < 20) {
            back_direction = 40;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RIGHT\n");
            sm.coor->move_status = "back up right";
        } else {
            if(sm.coor->delta > 0) {
                back_direction = 40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RIGHT - COMPASS\n");
                sm.coor->move_status = "back up right";
            } else {
                back_direction = -40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LEFT - COMPASS\n");
                sm.coor->move_status = "back up left";
            }
        }
        sbot->ignoreObstacle(true);
        sbot->setDirection(0);
        sbot->setSpeed(-3);
        sleep(4);
        sbot->setDirection(back_direction);
        sbot->setSpeed(3);
        sleep(3);
        sbot->setDirection(0);
        sbot->setSpeed(0);
        sleep(2);
        sbot->ignoreObstacle(false);
    } else if (see_obstacle) {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %ld\n",
               tmnow - found_obstacle);
        fflush(stdout);
        char str[20];
        sprintf(str, "obstacle %ld", tmnow - found_obstacle);
        sm.coor->move_status = str;
    }
    was_obstacle = see_obstacle;
    sm.coor->status_from_subroutines = see_obstacle;
}

int Subroutines::manageFinish(SensorManagement sm) {
    if (setstart == 0) {
        setstart = 1;
        startpos = sm.gdata;
    }

    // TODO ked 10sec stoji ze je v cieli vypni ho uz(ak je uspesne to
    // zastavenie)
    if (sm.angles.map == DBL_MAX) {
        finishcount++;
    } else {
        finishcount = 0;
    }
    if (finishcount > 60) {
        if (finnumber == 2) {
            printf("FINISH CONFIRMED with %d readings , TURNING OFF\n",
                   finishcount);
            sm.coor->move_status = "finish - turn off";
            sm.coor->status_from_subroutines = true;

            return true;
        } else {
            for (int ghe = 0; ghe < 7; ghe++)
                printf("FINISH REACHED GOING BACK TO START\n");
            sm.coor->move_status = "finish - go back";
            sm.coor->status_from_subroutines = true;
            // TODO set next dest
            sm.loc->setDestination(startpos);
            sm.loc->bestWay.clear();
            finnumber = 2;
        }
    }
    sm.coor->status_from_subroutines = false;
    return false;
}
