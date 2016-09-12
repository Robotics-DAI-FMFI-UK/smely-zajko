#include "Coordinate.h"

extern int away_from_left;
extern int away_from_right;

Coordinate::Coordinate() {
	wrong_dir =0;
    running_mean = 0.0;
    predicted_dir = 0.0;
}


int Coordinate::move(CvMat* predicted_data, SbotThread* sbot, double mapAngle, double imuAngle, EvalDireciton* ed, int* laserData ){ //returns direction as integer

//    if( !USE_LOCALIZATION ){
//        mapAngle = 0;
//    }

    int display_direction = 0;

    double delta;
    vector<double> vDist;
    vector<double> lDist;
    int isChodnik = 0;

    //threshold

    printf("evalLaser: ");
    for(int i=0; i<=ed->dir_count; i++){
        double f = ed->eval( predicted_data, i )-0.4;
        double g = ed->evalLaser( laserData, i );
        if( f<0){
            f=0;
        }else  if( f > 0 && g > 0.5 ){
            isChodnik = 1;
        }
        vDist.push_back( f );
        lDist.push_back( g );
        printf("%.3f ", g);
    }
    printf("\n");


    //in destination vicinity
    if (mapAngle == DBL_MAX){
        sbot->setDirection( 0 );
        sbot->setSpeed(0);
        return 0;
    }

    //delta
    if (mapAngle == DBL_MIN){
        delta = 0;
    } else {
     //   delta = ((int)(mapAngle /* - (imuData.xAngle/10) */ + 360))%360 ;
        delta = ((int)(mapAngle - imuAngle/10 + 360))%360 ;
    }
    if (delta >180) delta = delta - 360;

	//heading roughly the right way
	if (abs(delta) < 40){
		wrong_dir = 0;
	}
	//significantly off course(> 150deg), turn
	if (abs(delta)> 150 || wrong_dir )
        {
		//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Going in wrong direction, delta %f turning...\n",delta);
		wrong_dir = 1;
        if (delta>0)
        {
            if(autonomy){
                sbot->setDirection( 80 );
                sbot->setSpeed( 1 );
            }
            display_direction = 5;
        }else
        {
            if(autonomy){
                sbot->setDirection( -80 );
                sbot->setSpeed( 1 );
            }
            display_direction = -5;
        }


	}
        else if( isChodnik==0 ) //no valid direction from vision//TODO presun do subroutines
    {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Chodnik missing searching..\n");
        if(delta>0)
        {
            if(autonomy){
                sbot->setDirection( 40 );
                sbot->setSpeed( -1 );
            }
            display_direction = 5;
        }else
        {
            if(autonomy){
                sbot->setDirection( -40 );
                sbot->setSpeed( -1 );
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
          double f = vDist[i] * coeff * lDist[i]; //TODO x 1if hoku sez > 1.5m else 0.1
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

      predicted_dir = running_mean * 0.7 + sdir * 0.3;
      running_mean = (running_mean * 5.0 + predicted_dir) / 6.0;

      printf("Inferred dir: %d\tProposed dir: %f\n", sdir, predicted_dir);
      
      computed_dir = sdir;
      
      if (autonomy){
        sbot->setDirection( sdir );
        sbot->setSpeed(5);
      }
      display_direction = maxdir;
    }

    //printf("delta= %f\n", delta);

    return display_direction;
}

