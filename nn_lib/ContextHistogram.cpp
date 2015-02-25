#include "ContextHistogram.h"


using namespace std;

void ContextHistogram::set_params(vector<int> p, vector<int> netparams){
    stretch = false;
    pomer = (float)1/(float)10;
    if(p.size() > 0)
        pomer = (float)1/(float)p[0];
    if(p.size() > 1 && p[1]==1)
        stretch = true;
}

int ContextHistogram::getSize(){
     int lbins = 256*pomer, abins = 256*pomer, bbins = 256*pomer;
    if(stretch){
        //a,b values can range from 1 to 255 BUT when converting from RGB range is approx 80 - 220 so make it 60-230
        lbins = 256*pomer; abins = 170*pomer; bbins = 170*pomer;
    }
     return lbins + abins + bbins;
}

vector<float> ContextHistogram::getContext(IplImage* input){
    //histogram vela nul
    // Quantize the l to 256 levels(opencv converts the normal 0-100range to 0-256)
    // and the ab to 256 levels(opencv converts the normal -127 to 127 range to 0-256)
    int lbins = 256*pomer, abins = 256*pomer, bbins = 256*pomer;
    int lowbound = 0;
    if(stretch){
        //a,b values can range from 1 to 255 BUT when converting from RGB range is approx 80 - 220 so make it 60-230
        lbins = 256*pomer; abins = 170*pomer; bbins = 170*pomer;
        lowbound = 60;
    }
    else{
        
    }
    
    vector<int> ls(lbins,0);
    vector<int> as(abins,0);
    vector<int> bs(bbins,0);
    
    //cout<<ls[0]<<" l \n";
    CvScalar s;
    int l,a,b;
    for(int x = 0; x<input->width; x++){
        for(int y = 0; y<input->height; y++){
            s = cvGet2D(input,y,x);
            l = s.val[0]*pomer;
            a = (s.val[1]-lowbound)*pomer;
            b = (s.val[2]-lowbound)*pomer;
            //check vs bad values
            if(a < 0) a = 0;
            if(b < 0) b = 0;
            if(l > lbins-1)
                l = lbins-1;
            if(a > abins-1)
                a = abins-1;
            if(b > bbins-1)
                b = bbins-1;
            
            ls[l]++;
            as[a]++;
            bs[b]++;
        }
    }
    
    vector<float> res;
    float all = (float)(input->width * input->height);
    for(int i = 0; i< lbins;i++){
        res.push_back((float)ls[i]/all);
    }
    for(int i = 0; i< abins;i++){
        res.push_back((float)as[i]/all);
    }
    for(int i = 0; i< bbins;i++){
        res.push_back((float)bs[i]/all);
    }
    return res;
    
}