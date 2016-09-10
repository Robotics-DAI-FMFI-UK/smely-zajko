#include "ContextHistPlusRect.h"


using namespace std;

void ContextHistPlusRect::set_params(vector<int> p, vector<int> netparams){
    con1 = new ContextProbableRectangle();
    con2 = new ContextHistogram();
    //vector<int> ve = ;
    con1->set_params({8,10,1},  netparams);
    con2->set_params(p,  netparams);
}

int ContextHistPlusRect::getSize(){
    return con1->getSize() + con2->getSize();
}

vector<float> ContextHistPlusRect::getContext(IplImage* input){
    vector<float> a = con1->getContext(input);
    vector<float> b = con2->getContext(input);
    for(int i = 0; i < a.size();i++){
        b.push_back(a[i]);
    }
    return b;    
}