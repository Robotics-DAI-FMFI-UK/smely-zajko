/* 
 * File:   ModKMeans.h
 * Author: root
 *
 * Created on January 7, 2015, 11:06 PM
 */

#ifndef MODKMEANS_H
#define	MODKMEANS_H

#include "VisionModifier.h"


using namespace std;

class ModKMeans : public VisionModifier {
  protected:
    int k,blur,size;
  public:
    virtual void set_params (vector<int> p,vector<int> netparams);
    virtual cv::Mat getContext (IplImage* input);
    virtual int getSize();
};

#endif	/* MODKMEANS_H */

