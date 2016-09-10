#include "ContextProbableRectangle.h"

using namespace std;


void ContextProbableRectangle::set_params(vector<int> p, vector<int> netparams){
    a = 4;
    b = 10;
    multiple = false;
    if(p.size() > 1){
        a = p[0];
        b = p[1];//TODO do this as rectangle..
    }
    if(p.size() > 2){
        if(p[2] > 0)
            multiple = true;
        a = 8;b=10;
    }
}

int ContextProbableRectangle::getSize(){
    return 3;
}

vector<float> ContextProbableRectangle::getContext(IplImage* input){
    if(multiple)
        return getMultiple(input);
    else
        return getSimple(input);
    
}

vector<float> ContextProbableRectangle::getMultiple(IplImage* input){

   CvScalar s;
    float kl,ka,kb;
    
    vector< tuple<float,float,float> > vek;
    
    for(int r = 0; r<2; r++){ //riadok
        for(int st = 0; st<4; st++){ //stlpec
            kl = 0;ka=0;kb=0;

            for (int i = 0; i < input->width/a; i++) {        
                for (int j = 0; j < input->height/b; j++) {
                    
                    s = cvGet2D(input,   input->height - input->height/b +j - (input->height/b)*r     ,
                            input->width/(a/2) + i     +  st*(input->width/a) ); //get2D ma druhy parameter y a treti x

                    kl +=(float)s.val[0]/(float)255;
                    ka +=(float)s.val[1]/(float)255;
                    kb +=(float)s.val[2]/(float)255;


                }
            }
            kl = (kl /(float)((input->width/a)*(input->height/b)));
            ka = (ka /(float)((input->width/a)*(input->height/b)));
            kb = (kb /(float)((input->width/a)*(input->height/b)));
            vek.push_back(tuple<float,float,float>(kl,ka,kb));
            
        }
    }
    
    
    std::sort(begin(vek), end(vek), 
    [](tuple<float, float,float> const &t1, tuple<float, float,float> const &t2) {
        return get<1>(t1)*get<1>(t2) < get<1>(t2)*get<2>(t2); // vse je kladne sort podla obsahu euklidovskeho
    }
);
    tuple<float,float,float> median = vek[vek.size()/2];
    vector<float> res;
    res.push_back(get<0>(median));
    res.push_back(get<1>(median));
    res.push_back(get<2>(median));

    return res; 
}

vector<float> ContextProbableRectangle::getSimple(IplImage* input){

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
    res.push_back(kl /(float)((input->width/a)*(input->height/b)));
    res.push_back(ka /(float)((input->width/a)*(input->height/b)));
    res.push_back(kb /(float)((input->width/a)*(input->height/b)));

    return res; 
}