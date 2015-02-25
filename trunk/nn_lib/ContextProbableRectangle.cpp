#include "ContextProbableRectangle.h"

using namespace std;


void ContextProbableRectangle::set_params(vector<int> p, vector<int> netparams){
    a = 4;
    b = 10;
    if(p.size() > 1){
        a = p[0];
        b = p[1];//TODO do this as rectangle..
    }
}

int ContextProbableRectangle::getSize(){
    return 3;
}

vector<float> ContextProbableRectangle::getContext(IplImage* input){
    
    CvScalar s;
    float kl,ka,kb;
    for (int i = 0; i < input->width/a; i++) {
        for (int j = 0; j < input->height/b; j++) {
            s = cvGet2D(input, input->height - input->height/(b/2) +j, input->width/(a/2)-input->width/(a*2) + i); //get2D ma druhy parameter y a treti x

            kl +=(float)s.val[0]/(float)255;
            ka +=(float)s.val[1]/(float)255;
            kb +=(float)s.val[2]/(float)255;


        }
    }
    vector<float> res;
    res.push_back(kl /(float)((input->width/4)*(input->height/10)));
    res.push_back(ka /(float)((input->width/4)*(input->height/10)));
    res.push_back(kb /(float)((input->width/4)*(input->height/10)));

    return res;
}