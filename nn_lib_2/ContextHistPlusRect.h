/* 
 * File:   ContextHistPlusRect.h
 * Author: root
 *
 * Created on January 7, 2015, 10:59 PM
 */

#ifndef CONTEXTHISTPLUSRECT_H
#define	CONTEXTHISTPLUSRECT_H

#include "VisionContext.h"
#include "ContextHistogram.h"
#include "ContextProbableRectangle.h"

using namespace std;

class ContextHistPlusRect : public VisionContext {
  protected:
      
      ContextProbableRectangle* con1;
      ContextHistogram* con2;
  public:
    virtual void set_params (vector<int> p,vector<int> netparams);
    virtual vector<float> getContext (IplImage* input);
    virtual int getSize();
};


#endif	/* CONTEXTHISTOGRAM_H */

