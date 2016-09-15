#include "Coordinate.h"

extern int away_from_left;
extern int away_from_right;

Coordinate::Coordinate() {
    wrong_dir = 0;
    running_mean = 0.0;
    predicted_dir = 0.0;
    move_status = "standby";
    running_mean_weight = 0.0;
    speed_down_dst = 0.0;
}

// returns direction as integer
int Coordinate::move(CvMat* predicted_data, SbotThread* sbot, GpsAngles angles,
                     double imuAngle, EvalDirection* ed, int* laserData) {

    double mapAngle = angles.map;
    //    if( !USE_LOCALIZATION ){
    //        mapAngle = 0;
    //    }

    int display_direction = 0;

    double delta;
    vector<double> vDist;
    vector<double> lDist;
    int isChodnik = 0;

    // threshold

    //printf("evalLaser: ");
    double max_neural_dir = 0;
    double max_neural_dir_val = 0.0;
    for (int i = 0; i <= ed->dir_count; i++) {
        double f = ed->eval(predicted_data, i) - 0.4;
        double g = ed->evalLaser(laserData, i);
        if (f < 0) {
            f = 0;
            
        } else if (f > 0 && g > 0.5) {
            isChodnik = 1;
        }
        
        if (f > max_neural_dir_val) {
            max_neural_dir_val = f;
            max_neural_dir = i;
        }
        
        vDist.push_back(f);
        lDist.push_back(g);
        //printf("%.3f ", g);
    }
    neuron_dir = max_neural_dir;
    //printf("\n");

    // in destination vicinity
    if (mapAngle == DBL_MAX) {
        sbot->setDirection(0);
        sbot->setSpeed(0);
        return 0;
    }

    // delta
    if (mapAngle == DBL_MIN) {
        delta = 0;
    } else {
        //   delta = ((int)(mapAngle /* - (imuData.xAngle/10) */ + 360))%360 ;
        delta = ((int)(mapAngle - imuAngle / 10 + 360)) % 360;
    }
    if (delta > 180)
        delta = delta - 360;

    // heading roughly the right way
    if (abs(delta) < 40) {
        wrong_dir = 0;
    }
    // significantly off course(> 150deg), turn
    if (abs(delta) > 150 || wrong_dir) {
        if (!status_from_subroutines)
            move_status = "turning";
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Going in wrong direction,
        // delta %f turning...\n",delta);
        wrong_dir = 1;
        if (delta > 0) {
            if (autonomy) {
                sbot->setDirection(80);
                sbot->setSpeed(1);
            }
            display_direction = 5;
        } else {
            if (autonomy) {
                sbot->setDirection(-80);
                sbot->setSpeed(1);
            }
            display_direction = -5;
        }

    } else if (isChodnik == 0) {
        // no valid direction from vision
        // TODO presun do subroutines

        if (!status_from_subroutines)
            move_status = "searching";

        printf("!!!!!!!!!!!!!!!!!!!!!!! Chodnik missing searching..\n");
        if (delta > 0) {
            if (autonomy) {
                sbot->setDirection(40);
                sbot->setSpeed(-1);
            }
            display_direction = 5;
        } else {
            if (autonomy) {
                sbot->setDirection(-40);
                sbot->setSpeed(-1);
            }
            display_direction = -5;
        }
    } else {
        if (!status_from_subroutines)
            move_status = "running";
        if (delta > 90) {
            delta = 89.3;
        } else if (delta < -90) {
            delta = -89.3;
        }
        delta /= 18.0;

        double fmax = -1;
        int maxdir = 5;
        move_probs.clear();
        for (int i = 0; i <= ed->dir_count; i++) {
            double coeff = 5 - abs(delta - (i - 5));
            if (coeff < 0.0)
                coeff = 0.1;
            coeff /= 12.0; // vyskusat 5, 12 ...
            coeff += 1.0;
            // TODO x 1if hoku sez > 1.5m else 0.1
           // lDist[i] = 1.0;
            double f = vDist[i] * coeff * lDist[i];
            move_probs.push_back(f);
            if (f > fmax) {
                fmax = f;
                maxdir = i;
            }
        }
        for(int i = 0; i < move_probs.size(); i++) {
            move_probs[i] /= fmax;
            move_probs[i] = 1 - move_probs[i];
        } 

        int sdir = (maxdir - 5) * 8;
        //      sdir -= 3;
        if (away_from_left)
            sdir += 20;
        if (sdir > 40)
            sdir = 40;
        if (away_from_right)
            sdir -= 20;
        if (sdir < -40)
            sdir = -40;

        predicted_dir = (running_mean * running_mean_weight) + (sdir * (1-running_mean_weight));
        running_mean = (running_mean * 3.0 + predicted_dir) / 4.0;

        //printf("Inferred dir: %d\tProposed dir: %f\n", sdir, predicted_dir);

        computed_dir = sdir;

        if (autonomy) {
            sbot->setDirection(predicted_dir);
            printf("%.10f %.10f\n", angles.dstToHeadingPoint, speed_down_dst);
            if (angles.dstToHeadingPoint <= speed_down_dst) {
                sbot->setSpeed(5);
                printf("setSpeed: 5\n");
            } else {
                sbot->setSpeed(10);
                printf("setSpeed: 10\n");
            }
        }
        display_direction = maxdir;
    }

    // printf("delta= %f\n", delta);

    return display_direction;
}
