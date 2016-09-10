/* 
 * File:   VisionModifier.h
 * Author: root
 *
 * Created on January 7, 2015, 7:20 PM
 */

#ifndef VISIONMODIFIER_H
#define	VISIONMODIFIER_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

class VisionModifier {
  protected:
    int width, height;
  public:
    virtual void set_params (vector<int> p,vector<int> netparams){
        
    }    
    virtual cv::Mat getContext (IplImage* input){
        return cvCreateImage(cvSize(1,1),1,1); 
    }
    
    virtual int getSize(){
        return 0; 
    }
};


#endif	/* VISIONMODIFIER_H */

