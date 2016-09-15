#ifndef EVALDIRECTION_H
#define EVALDIRECTION_H

#define LASER_VALS_START 420
#define LASER_VALS_END 660

#define MIN_OBSTACLE_DISTANCE_MM 300
#define MAX_OBSTACLE_DISTANCE_MM 1600

#define PROB_GO 1.0
#define PROB_NO_GO 0.001

#include <cv.h>
#include <vector>

using namespace std;

class EvalDirection {
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

    EvalDirection(int triangle_w, int triangle_h, int dir_count, int frame_w,
                  int frame_h);

    double eval(CvMat* frame, int direction);
    double evalLaser(int* laserData, int direction);
    int get_best(CvMat* frame);
    vector<int> directions;

    virtual ~EvalDirection();

  private:
};

#endif /* EVALDIRECTION_H */
