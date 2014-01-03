/* 
 * File:   Coordinate.cpp
 * Author: myron
 * 
 * Created on Piatok, 2011, máj 6, 12:10
 */

#include "Coordinate.h"

extern int away_from_left;
extern int away_from_right;

Coordinate::Coordinate() {
}


int Coordinate::move(CvMat* predicted_data, SbotThread* sbot, double mapAngle, double imuAngle, EvalDireciton* ed ){ //returns direction as integer

//    if( !USE_LOCALIZATION ){
//        mapAngle = 0;
//    }

    int display_direction = 0;

    double delta;
    vector<double> vDist;
    int isChodnik = 0;

    //threshold

    for(int i=0; i<=ed->dir_count; i++){
        double f = ed->eval( predicted_data, i )-0.4;

        if( f<0){
            f=0;
        }else  if( f>0 ){
            isChodnik = 1;
        }
        vDist.push_back( f );
    }
 

    //delta
    if( mapAngle==DBL_MIN ){
        delta = 0;
    }else{
     //   delta = ((int)(mapAngle /* - (imuData.xAngle/10) */ + 360))%360 ;
        delta = ((int)(mapAngle - imuAngle/10 + 360))%360 ;
    }
    if (delta >180) delta = delta - 360;



    if( isChodnik==0 ) //no valid direction from vision
    {
        if(delta>0)
        {
            if(autonomy){
                sbot->setDirection( 40 );
                sbot->setSpeed( 1 );
            }
            display_direction = 5;
        }else
        {
            if(autonomy){
                sbot->setDirection( -40 );
                sbot->setSpeed( 1 );
            }
            display_direction = -5;
        }
    }
    else
    {
        if(delta>90){
            delta = 89.3;
        }else if(delta<-90){
            delta = -89.3;
        }
      delta /= 18.0;

      double fmax = -1;
      int maxdir = 5;

      for(int i=0; i<=ed->dir_count; i++){
          double coeff = 5 - abs(delta-(i-5) );
          if (coeff < 0.0) coeff = 0.1;
          coeff /= 25.0;//vyskusat 5, 12 ...
          coeff += 1.0;
          double f = vDist[i]*coeff;
          if( f>fmax ){
              fmax = f;
              maxdir = i;
        }
      }


      int sdir = (maxdir-5)*8;
//      sdir -= 3;
      if (away_from_left) sdir += 20;
      if (sdir > 40) sdir = 40;
      if (away_from_right) sdir -= 20;
      if (sdir < -40) sdir = -40;
      if (autonomy){
        sbot->setDirection( sdir );
        sbot->setSpeed(4);
      }
      display_direction = maxdir;
    }

    //printf("delta= %f\n", delta);

    return display_direction;
}

