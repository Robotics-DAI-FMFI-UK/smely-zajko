#include "EvalDireciton.h"
#include <stdio.h>
#include <highgui.h>

EvalDireciton::EvalDireciton( int triangle_w, int triangle_h, int dir_count, int frame_w, int frame_h ) {
    this->dir_count = dir_count;
    this->triangle_w = triangle_w;
    this->triangle_h = triangle_h;
    this->frame_w = frame_w;
    this->frame_h = frame_h;

    this->mask = NULL;

    A = cvPoint( (frame_w-triangle_w)/2 , frame_h);
    B = cvPoint( frame_w-(triangle_w/2) , frame_h);
    C = cvPoint( frame_w/2, frame_h - triangle_h);

    S = (triangle_h*triangle_w)/2;
}

EvalDireciton::~EvalDireciton() {
}

void EvalDireciton::set_mask(){

    if( mask==NULL){
        mask = cvCreateMat( frame_h, frame_w, CV_32F );
    }

    cvSetZero( mask );

    cvLine( mask, A, C, cvScalar(1));
    cvLine( mask, C, B, cvScalar(1));

    cvFloodFill( mask,  cvPoint( (A.x + B.x + C.x)/3  , (A.y + B.y + C.y)/3 ) , cvScalar(1) );

    //cvShowImage( "debug", mask );
}

double EvalDireciton::comp_sum( CvMat* frame){

    CvMat* tmp_frame2 = cvCreateMat( frame->rows, frame->cols, frame->type );

    set_mask();
    //printf("widthframe %d maskwi %d \n", frame->width, mask->width);
    cvAnd( frame, mask , tmp_frame2 );
    
    CvScalar s = cvSum( tmp_frame2 );

    //printf("cvsum %f %f %f \n", s.val[0], s.val[1], s.val[2]);

    //printf("sc %f percent:%f \n", s.val[0], (s.val[0]/S)*100 );

    cvReleaseMat( &tmp_frame2 );
    return s.val[0];
}

double EvalDireciton::eval( CvMat* frame, int direction ){
    if(direction > dir_count || direction<0){
        printf("bad parameter direciton in EvalDireciton::eval");
        return -1;
    }

    int sizeC = frame_w / dir_count;
    int sizeA = (frame_w-triangle_w) / (dir_count);
    A.x = sizeA*direction;
    B.x = frame_w - sizeA*(dir_count - direction);
    C.x = sizeC*direction;

    //printf("tr d: %d A: %d B: %d C: %d \n", direction,A.x, B.x, C.x);

    double r = comp_sum( frame );

    //printf("eval direction: %d sum: %f  \n", direction,r);

    double result = r/S;

    return result;//*(5-abs(direction-5))/25;

}

int EvalDireciton::get_best( CvMat* frame ){
    double best = 0;
    int best_i = 0;

    double c;

    for(int i=0; i<=dir_count; i++){
        c = eval(frame, i);
        if( c > best){
            best = c;
            best_i = i;
        }
    }

    return best_i;
}