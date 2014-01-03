/* 
 * File:   main.cpp
 * Author: myron
 *
 * Created on Sobota, 2010, august 14, 17:23
 */

#include <cv.h>
#include <highgui.h>

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv/highgui.h>
#include <sys/time.h>

#include "VisionBase.h"
#include "VisionRegionReduced.h"
#include "VisionRegion.h"
#include "VisionHistogram.h"

#include "EvalDireciton.h"
#include "SbotThread.h"
#include "BindSerialPorts.h"
#include "ImuThread.h"
#include "GpsThread.h"
//#include "PhoneThread.h"
#include "Coordinate.h"
//#include "JoystickThread.h"

const bool START_CAM = 1;
const bool USE_LOCALIZATION = 0;
//const bool START_GPS = 0;
//const bool START_IMU = 0;
//const bool START_SBOT = 0;
//const bool START_JOYSTICK = 0;
bool START_TIMER = 1;

//VisionHistogram nn(320, 240, 6, 6, 12);  //(720, 576, 6, 6, 12);
//VisionBase nn(320, 240, 4, 4);
VisionBase nn(320, 240, 6, 6);

EvalDireciton ed( nn.image_width*0.4, nn.image_height*0.7, 10, nn.image_width, nn.image_height );

FILE* mainLog;
int log_counter=0;
bool autonomy = true;

void init_video(){
    FILE* mplayer;
    char *command=(char *)"mplayer tv:// -tv driver=v4l2:device=/dev/video1:immediatemode=0:normid=0:outfmt=YUY2:input=1";

    if ( !(mplayer = (FILE*)popen(command,"r")) ){
        // If fpipe is NULL
        perror("Problems with pipe");
        exit(1);
    }

    char b[1024];

    while ( fgets( b, sizeof b, mplayer) ){
        if( strcmp( b, "Starting playback...\n" )==0 ){
            break;
        }
        printf("%s", b );
    }

    pclose( mplayer );

    system("clear");
}

void timer(){
    time_t t = time(NULL);

    printf("current time: %s \n", ctime( &t) );

    struct tm x;
    printf("enter start time\n");
    //char *s = "12:00:00";
    char s[30];
    fgets( s, 30, stdin );

    s[strlen(s)-1]='\0';

    x = *localtime(&t);

    strptime( s, "%T", &x );
    time_t time1 = mktime( &x );

    printf("current: %d, start: %d, (%s)\n", (int)time(NULL), (int)time1, s);

    while( time(NULL)< time1 ){
        printf("%d seconds to start\n", (int)(time1 - time(NULL)) );
        sleep(1);
    }
}

Point pixel;
int clicked;

void loc_mouse_callback(int event, int x, int y, int flags, void* param)
{
    if( event == CV_EVENT_LBUTTONDOWN || ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)  ) )
    {
      pixel.x = x;
      pixel.y = y;
      clicked = 1;
    }

}

void log_data(SbotData sdata, Ll gdata, ImuData idata, double mapAngle )
{
    time_t t;
    time(&t);
    fprintf(mainLog, " %ld %d %d %d %d %d %d %d %d %d %d %d %f %f %f %f %f %f\n", t, sdata.lstep, sdata.rstep, sdata.lspeed,
        sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR,
        idata.xAngle, idata.yAngle, idata.zAngle,
        gdata.longitude, gdata.latitude, mapAngle);

    fflush(mainLog);

    if(log_counter<3){
        log_counter++;
    }else{
        log_counter = 0;
        printf("data > %d %d %d %d %d %d %d %d %d %d %d \n", sdata.lstep, sdata.rstep, sdata.lspeed,
           sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR);
    }
}

/*
 * 
 */

int main(int argc, char** argv) {
    IplImage* localizationFrame;

    setlocale(LC_ALL, "C");
    time_t t;
    time(&t);

    char logname[64];
    sprintf(logname, "../logs/%ld.log", t);
    mainLog = fopen( logname, "w");
    if( mainLog == NULL ){
        printf("log cannot be created.\n");
        exit(-1);
    }

    IplImage* empty_frame = cvCreateImage( cvSize( 360, 240 ), 8, 3);

    cvNamedWindow( "camera", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "path", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "localization", CV_WINDOW_AUTOSIZE );

    cvShowImage( "camera", empty_frame );
    cvShowImage( "path", empty_frame );
    cvShowImage( "localization", empty_frame );

    cvMoveWindow( "camera", 1000, 30 );
    cvMoveWindow( "path", 670, 30 );
    cvMoveWindow( "localization", 620, 299 );

    LocalizationAndPlaning loc(800,400);
    
    //loc.readMap( "../maps/wien.osm" );
    //loc.readMap( (char *)"../maps/stromovka.osm" );
    loc.readMap( (char *)"../maps/lodz.osm" );

    loc.readDestination( (char *)"../destination.txt");
    cvSetMouseCallback( "localization", loc_mouse_callback, NULL );


    printf("!!!!!!!!map only? [m]\n");
    fflush(stdout);
    sleep(2);
    int mapkey = cvWaitKey(3500);
    if ('m' == mapkey)
    {
        printf("map only...\n");
        localizationFrame = loc.getGui();
        cvShowImage( "localization", localizationFrame );
        cvWaitKey(33);
        while (true) {
            if (clicked)
            {
                Ll p = loc.reverse(pixel);
                printf("click: long=%lf lat = %lf\n", p.longitude, p.latitude);
                fflush(stdout);
                clicked = 0;
            }
            cvWaitKey(10);
        }
    }
    else printf("%d %c", mapkey, mapkey);

    CvCapture* capture;

    if( START_CAM ){
        init_video();
        printf("mplayer init video done.\n");

        capture = cvCaptureFromCAM(1);  //(1);
    }else{ 
        //debug
        //capture = cvCreateFileCapture( "../../video/b23.avi");
        capture = cvCreateFileCapture( "../../stromovka-friday/236.jpg");

        }
     
    Coordinate coor;




    GpsThread gps;
    ImuThread imu;
    SbotThread sbot;
//    PhoneThread phone(&sbot);

    BindSerialPorts bs;

    bs.bind( &sbot, &gps, &imu );

    //sbot.init();
    gps.init();
    imu.init();

    sbot.run();

    sleep(1);
    sbot.setSpeed(1);
    sleep(1);
    sbot.setSpeed(0);

    gps.run();
    imu.run();
    //phone.run();

    IplImage* frame;
    setlocale(LC_ALL, "C");
//    nn.load("../1307266316.net");//1304670470.net
//    nn.load("../1316258985.net");//1304670470.net
    nn.load("../1347706382.net");
    CvMat* predicted_data;

    autonomy = true;


    int frame_counter = 0;

    bool was_obstacle = false;
    long found_obstacle;

    while(1) {
        frame = cvQueryFrame( capture );
        if( !frame ){
            printf("no frame, exiting..\n");
            break;
        }

        if(frame_counter<2){
            frame_counter++;
            continue;
        }
        frame_counter=0;

        IplImage* tmp_frame = cvCreateImage( cvSize( 360, 240 ), frame->depth,frame->nChannels);

        cvResize( frame, tmp_frame );

        cvFlip( tmp_frame, tmp_frame, 0);
        cvFlip( tmp_frame, tmp_frame, 1);

        if(START_TIMER){
           timer();
           START_TIMER = 0;
        }

        predicted_data = nn.predict( tmp_frame );
        //predicted_data = nn.nn_random_predict( tmp_frame, 1000 );

        //
        SbotData sdata = sbot.getData();
        ImuData idata = imu.getData();
        Ll gdata = gps.getData();

        if (sdata.obstacle && (!was_obstacle))
        {
            time_t tobs = time(NULL);
            found_obstacle = tobs;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
        }
        time_t tmnow = time(NULL);
        if ((sdata.obstacle) && (tmnow - found_obstacle > 30))
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
            found_obstacle = found_obstacle +60;
            // cuvat alebo aspon tocit!!
            sbot.ignoreObstacle(true);
            sbot.setDirection( 0 );
            sbot.setSpeed( -2 );
            sleep(4);
            sbot.setDirection( -40 );
            sbot.setSpeed( 1 );
            sleep(2);
            sbot.setDirection( 0 );
            sbot.setSpeed( 0 );
            sleep(2);
            sbot.ignoreObstacle(false);
        }
        else if (sdata.obstacle)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %ld\n", tmnow - found_obstacle);
            fflush(stdout);
        }
        was_obstacle = sdata.obstacle;

        GpsAngles a = loc.update( gdata);

        log_data( sdata, gdata, idata, a.map);

        int display_direction = coor.move(predicted_data, &sbot, a.map, idata.xAngle, &ed );

        //draw result
        int sizeC = ed.frame_w / ed.dir_count;
        
        //cvLine( predicted_data, cvPoint( sizeC*b, (ed.frame_h-ed.triangle_h) ), cvPoint( ed.frame_w/2, ed.frame_h ), cvScalar( 0, 255 ) );
        cvLine( tmp_frame, cvPoint( sizeC*display_direction, (ed.frame_h-ed.triangle_h) ), cvPoint( ed.frame_w/2, ed.frame_h ), cvScalar( 0, 0,255 ), 5);

        localizationFrame = loc.getGui();

        cvShowImage( "camera", tmp_frame );
        cvShowImage( "path", predicted_data );
        cvShowImage( "localization", localizationFrame );

        cvReleaseMat( &predicted_data );
        cvReleaseImage( &tmp_frame );
        cvReleaseImage( &localizationFrame );

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    sbot.stop();
    gps.stop();
    imu.stop();
    
    cvReleaseCapture( &capture );
    cvDestroyWindow( "camera" );
    cvDestroyWindow( "debug" );
    cvDestroyWindow( "localization" );

    fclose(mainLog);

    return 0;
}

