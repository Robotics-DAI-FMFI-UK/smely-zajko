#include "EvalDireciton.h"
#include <highgui.h>
#include <stdio.h>

EvalDireciton::EvalDireciton(int triangle_w, int triangle_h, int dir_count,
                             int frame_w, int frame_h) {
    this->dir_count = dir_count;
    this->triangle_w = triangle_w;
    this->triangle_h = triangle_h;
    this->frame_w = frame_w;
    this->frame_h = frame_h;

    this->mask = NULL;

    A = cvPoint((frame_w - triangle_w) / 2, frame_h);
    B = cvPoint(frame_w - (triangle_w / 2), frame_h);
    C = cvPoint(frame_w / 2, frame_h - triangle_h);

    S = (triangle_h * triangle_w) / 2;

    this->laser_vals_per_direction =
        (LASER_VALS_END - LASER_VALS_START) / dir_count;
}

EvalDireciton::~EvalDireciton() {}

void EvalDireciton::set_mask() {

    if (mask == NULL) {
        mask = cvCreateMat(frame_h, frame_w, CV_32F);
    }

    cvSetZero(mask);

    cvLine(mask, A, C, cvScalar(1));
    cvLine(mask, C, B, cvScalar(1));

    cvFloodFill(mask, cvPoint((A.x + B.x + C.x) / 3, (A.y + B.y + C.y) / 3),
                cvScalar(1));

    // cvShowImage( "debug", mask );
}

double EvalDireciton::comp_sum(CvMat* frame) {

    CvMat* tmp_frame2 = cvCreateMat(frame->rows, frame->cols, frame->type);

    set_mask();
    // printf("widthframe %d maskwi %d \n", frame->width, mask->width);
    cvAnd(frame, mask, tmp_frame2);

    CvScalar s = cvSum(tmp_frame2);

    // printf("cvsum %f %f %f \n", s.val[0], s.val[1], s.val[2]);

    // printf("sc %f percent:%f \n", s.val[0], (s.val[0]/S)*100 );

    cvReleaseMat(&tmp_frame2);
    return s.val[0];
}

double EvalDireciton::eval(CvMat* frame, int direction) {
    if (direction > dir_count || direction < 0) {
        printf("bad parameter direciton in EvalDireciton::eval");
        return -1;
    }

    int sizeC = frame_w / (dir_count + 1);
    int sizeA = (frame_w - triangle_w) / (dir_count + 1);
    A.x = sizeA * direction;
    B.x = frame_w - sizeA * (dir_count - direction);
    C.x = sizeC * direction;

    // printf("tr d: %d A: %d B: %d C: %d \n", direction,A.x, B.x, C.x);

    double r = comp_sum(frame);

    //    cvLine(frame, cvPoint(A.x, A.y), cvPoint(C.x, C.y), cvScalar(0, 0, 0),
    //    1);
    //    cvLine(frame, cvPoint(C.x, C.y), cvPoint(B.x, B.y), cvScalar(1, 1, 1),
    //    1);

    // printf("eval direction: %d sum: %f  \n", direction,r);

    double result = r / S;

    return result; //*(5-abs(direction-5))/25;
}

int EvalDireciton::get_best(CvMat* frame) {
    double best = 0;
    int best_i = 0;

    double c;

    for (int i = 0; i <= dir_count; i++) {
        c = eval(frame, i);
        if (c > best) {
            best = c;
            best_i = i;
        }
    }

    return best_i;
}

// TODO eval direction from hokuyo
double EvalDireciton::evalLaser(int* laserData, int direction) {

    direction = 10 - direction;
    int start = LASER_VALS_START + (direction * laser_vals_per_direction);
    int end = LASER_VALS_START + ((direction + 1) * laser_vals_per_direction);

    int min_distance = laserData[start];
    for (int i = start; i < end; ++i) {
        int curr_distance = laserData[i];
        if (curr_distance <= MIN_OBSTACLE_DISTANCE_MM) {
            continue;
        }

        if (curr_distance < min_distance) {
            min_distance = curr_distance;
        }
    }

    if (direction < dir_count / 3) {
        for (int i = 180; i < LASER_VALS_START; ++i) {
            int curr_distance = laserData[i];
            if (curr_distance <= MIN_OBSTACLE_DISTANCE_MM) {
                continue;
            }

            if (curr_distance < min_distance) {
                min_distance = curr_distance;
            }
        }
    } else if (direction > 2 * dir_count / 3) {
        for (int i = LASER_VALS_END; i < 901; ++i) {
            int curr_distance = laserData[i];
            if (curr_distance <= MIN_OBSTACLE_DISTANCE_MM) {
                continue;
            }

            if (curr_distance < min_distance) {
                min_distance = curr_distance;
            }
        }
    }

    if (min_distance < MAX_OBSTACLE_DISTANCE_MM) {
        return PROB_NO_GO;
    } else {
        return PROB_GO;
    }
}