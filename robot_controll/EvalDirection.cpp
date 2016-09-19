#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include <highgui.h>

#include "EvalDirection.h"
#include "HokuyoThread.h"


int alpha_to_ray(double alpha) {
    return (int) ((90.0 - alpha) * 4.0 + 0.5) + 180;
}

double ray_to_alpha(int ray) {
    return 90.0 - (ray - 180.0) / 4.0;
}

double dir_to_alpha(int dir) {
    return 6.25 * (dir - 5.0);
}

int alpha_to_dir(double alpha) {
    return ((int) (alpha / 6.25 + 0.5)) + 5;
}

EvalDirection::EvalDirection(int triangle_w, int triangle_h, int dir_count,
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

    directions.push_back(650);
    directions.push_back(630);
    directions.push_back(610);
    directions.push_back(585);
    directions.push_back(560);
    directions.push_back(525);
    directions.push_back(500);
    directions.push_back(470);
    directions.push_back(445);
    directions.push_back(420);
    directions.push_back(400);

    // y = par_a * x^2 + par_c

    double par_c = PARABOLA_RANGE_FORWARD;
    double par_a = -PARABOLA_RANGE_FORWARD / (PARABOLA_RANGE_SIDE * PARABOLA_RANGE_SIDE);

    //printf("parabola a = %f, c = %f\n", par_a, par_c);

    for (int ray = 180; ray <= 900; ray++) {
        double alpha = (ray_to_alpha(ray) + 90.0) / 180.0 * M_PI;
        //printf("sqrt of %f\n", sin(alpha) * sin(alpha) - 4 * par_a * par_c * cos(alpha) * cos(alpha));
        //printf("dividing by %f\n", (2 * par_a * cos(alpha) * cos(alpha)));
        if (ray == 540) max_range[ray] = PARABOLA_RANGE_FORWARD;
        else max_range[ray] = -((0.0 - sin(alpha) +
                sqrt(sin(alpha) * sin(alpha) - 4 * par_a * par_c * cos(alpha) * cos(alpha)))
                / (2 * par_a * cos(alpha) * cos(alpha)));
        //printf("%d   %f   %f\n", ray, alpha, max_range[ray]);
    }
}

EvalDirection::~EvalDirection() {
}

void EvalDirection::set_mask() {

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

double EvalDirection::comp_sum(CvMat* frame) {

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

double EvalDirection::eval(CvMat* frame, int direction) {
    if (direction > dir_count || direction < 0) {
        printf("bad parameter direciton in EvalDireciton::eval");
        return -1;
    }

    double sizeC = frame_w / (double) (dir_count);
    double sizeA = (frame_w - triangle_w) / (double) (dir_count);
    A.x = (int) (sizeA * direction + 0.5);
    B.x = (int) (frame_w - sizeA * (dir_count - direction) + 0.5);
    C.x = (int) (sizeC * direction + 0.5);

    // printf("tr d: %d A: %d B: %d C: %d \n", direction,A.x, B.x, C.x);

    double r = comp_sum(frame);

    //cvLine(frame, cvPoint(A.x, A.y), cvPoint(C.x, C.y), cvScalar(0, 0, 0), 1);
    //cvLine(frame, cvPoint(C.x, C.y), cvPoint(B.x, B.y), cvScalar(0, 0, 0), 1);

    // printf("eval direction: %d sum: %f  \n", direction,r);

    double result = r / S;

    return result; //*(5-abs(direction-5))/25;
}

int EvalDirection::get_best(CvMat* frame) {
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

double EvalDirection::evalLaser(int* laserData, int direction) {

//    int direction_laser_begin = max(180, directions[direction] - 360);
    int direction_laser_begin = directions[direction] - 360;

//    int direction_laser_end = min(900, directions[direction] + 360);
    int direction_laser_end = directions[direction] + 360;

    double sum = 0;
    double sum1 = 0;

    //printf("------------------------ dir=%d\n", direction);

    for (int i = direction_laser_begin; i < direction_laser_end; i++) {
        
        int relative_ray = i - (directions[direction] - RANGE_DATA_COUNT / 2);
        double normalized_alpha = ray_to_alpha(relative_ray) / 90.0;

        double ray_weight = (1 - fabs(normalized_alpha));
        ray_weight *= ray_weight;
        ray_weight *= ray_weight;
        sum += ray_weight;
        double laser_limited = (laserData[i] > max_range[relative_ray]) ? (PARABOLA_RANGE_FORWARD) : (laserData[i]);
        sum1 += ray_weight * laser_limited / PARABOLA_RANGE_FORWARD; // max_range[i];
 
        //printf("%d: %lf  %lf\n", i, (1 - fabs(normalized_alpha)), (1 - fabs(normalized_alpha)) * laser_limited / PARABOLA_RANGE_FORWARD);
    }
    

    double result = sum1 / sum;

    return result;

    /*
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
     */
}