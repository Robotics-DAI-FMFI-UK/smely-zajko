#ifndef EVALDIRECITON_H
#define EVALDIRECITON_H

#define LASER_VALS_START 420
#define LASER_VALS_END 660

#define MIN_OBSTACLE_DISTANCE_MM 300
#define MAX_OBSTACLE_DISTANCE_MM 1600

#define PROB_GO 1.0
#define PROB_NO_GO 0.1

#include <cv.h>

class EvalDireciton {
    CvPoint A, B, C;
    int S;

    CvMat* mask;

    int laser_vals_per_direction;

    void set_mask();
    double comp_sum(CvMat* frame);

  public:
    int triangle_w;
    int triangle_h;
    int dir_count;
    int frame_w;
    int frame_h;

    EvalDireciton(int triangle_w, int triangle_h, int dir_count, int frame_w,
                  int frame_h);

    double eval(CvMat* frame, int direction);
    double evalLaser(int* laserData, int direction);
    int get_best(CvMat* frame);

    virtual ~EvalDireciton();

  private:
};

#endif /* EVALDIRECITON_H */
