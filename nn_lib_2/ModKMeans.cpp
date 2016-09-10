#include "ModKMeans.h"
using namespace std;

void ModKMeans::set_params(vector<int> p, vector<int> netparams){
    
    k=8;
    blur = 5;
    size = 1;
    if(p.size()>0)
        k = p[0];
    if(p.size()>1)
        blur = p[1];
    if(netparams.size() > 3)
        size = netparams[2]*netparams[3];
}

int ModKMeans::getSize(){
    return size;
}

cv::Mat ModKMeans::getContext(IplImage* src){
    
    
    vector<cv::Mat> imgRGB;
    cv::split(src,imgRGB);    
    int n = src->width*src->height;
    cv::Mat img3xN(n,3,CV_8U);
    for(int i=0;i!=3;++i) { 
        cv::blur(imgRGB[i], imgRGB[i], cv::Size(blur,blur));
        imgRGB[i].reshape(1,n).copyTo(img3xN.col(i));
    }
    img3xN.convertTo(img3xN,CV_32F);
    cv::Mat bestLabels;
    
    cv::kmeans(img3xN,k,bestLabels,cv::TermCriteria(),3,cv::KMEANS_RANDOM_CENTERS );
    bestLabels= bestLabels.reshape(0,src->height);
    cv::convertScaleAbs(bestLabels,bestLabels,int(255/k));//convert to 0-255 with 255/k steps for labels

    return bestLabels;
}        