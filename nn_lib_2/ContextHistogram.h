/* 
 * File:   ContextHistogram.h
 * Author: root
 *
 * Created on January 7, 2015, 10:59 PM
 */

#ifndef CONTEXTHISTOGRAM_H
#define	CONTEXTHISTOGRAM_H

#include "VisionContext.h"


using namespace std;

class ContextHistogram : public VisionContext {
  protected:
    int k;
    float pomer;//0.1
    bool stretch;
  public:
    virtual void set_params (vector<int> p,vector<int> netparams);
    virtual vector<float> getContext (IplImage* input);
    virtual int getSize();
};


#endif	/* CONTEXTHISTOGRAM_H */

