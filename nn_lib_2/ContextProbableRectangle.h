/* 
 * File:   ContextProbableRectangle.h
 * Author: root
 *
 * Created on January 7, 2015, 11:04 PM
 */

#ifndef CONTEXTPROBABLERECTANGLE_H
#define	CONTEXTPROBABLERECTANGLE_H

#include "VisionContext.h"

using namespace std;

class ContextProbableRectangle : public VisionContext {
  protected:
    int a,b;
    bool multiple;
    virtual vector<float> getMultiple(IplImage* input);
    virtual vector<float> getSimple(IplImage* input);
  public:
    virtual void set_params (vector<int> p,vector<int> netparams);
    virtual vector<float> getContext (IplImage* input);
    virtual int getSize();
};


#endif	/* CONTEXTPROBABLERECTANGLE_H */

