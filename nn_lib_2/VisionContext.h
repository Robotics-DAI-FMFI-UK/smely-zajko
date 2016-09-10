/* 
 * File:   VisionContext.h
 * Author: root
 *
 * Created on January 7, 2015, 7:18 PM
 */

#ifndef VISIONCONTEXT_H
#define	VISIONCONTEXT_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

class VisionContext {
  protected:
    int width, height;
  public:
    virtual void set_params (vector<int> p,vector<int> netparams){
    }
    virtual vector<float> getContext (IplImage* input){
        vector<float>v; 
        return v;
    }
    virtual int getSize(){ 
        return 0; 
    }
};

#endif	/* VISIONCONTEXT_H */

