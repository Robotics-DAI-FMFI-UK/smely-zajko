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

#include "SbotThread.h"
#include "BindSerialPorts.h"
#include "ImuThread.h"

const bool START_CAM = 1;
const bool START_TIMER = 1;


FILE* mainLog;
int log_counter=0;

void init_video(){
    FILE* mplayer;
    char *command="mplayer tv:// -tv driver=v4l2:device=/dev/video1:immediatemode=0:normid=0:outfmt=YUY2:input=1";

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

void log_data(SbotData sdata, ImuData idata ){
    time_t t;
    time(&t);
    fprintf(mainLog, " %ld %d %d %d %d %d %d %d %d %d %d %d %f %f %f\n", t, sdata.lstep, sdata.rstep, sdata.lspeed,
        sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR,
        idata.xAngle, idata.yAngle, idata.zAngle);

    fflush(mainLog);

    if(log_counter<3){
        log_counter++;
    }else{
        log_counter = 0;
        printf("data > %d %d %d %d %d %d %d %d %d %d %d \n", sdata.lstep, sdata.rstep, sdata.lspeed,
           sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR);
    }
}

int main(int argc, char** argv) {

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
    cvShowImage( "camera", empty_frame );

    cvWaitKey(33);
    
    CvCapture* capture;

    if( START_CAM ){
        init_video();
        printf("mplayer init video done.\n");

        capture = cvCaptureFromCAM(1);  //(1);
    }else{
        //debug
        capture = cvCreateFileCapture( "../../video/b23.avi");
    }
    ImuThread imu;
    SbotThread sbot;

    BindSerialPorts bs;

    bs.bind( &sbot, &imu );

    imu.init();

    sbot.run();

    sleep(1);
    sbot.setSpeed(1);
    sleep(1);
    sbot.setSpeed(0);

    imu.run();

    if(START_TIMER){
        timer();
    }

    IplImage* frame;
    setlocale(LC_ALL, "C");

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

        SbotData sdata = sbot.getData();
        ImuData idata = imu.getData();

        if (sdata.obstacle && (!was_obstacle))
        {
            time_t tobs = time(NULL);
            found_obstacle = tobs;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
        }
        time_t tmnow = time(NULL);
        if ((sdata.obstacle) && (tmnow - found_obstacle > 40))
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
            found_obstacle = found_obstacle +120;
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
            sbot.ignoreObstacle(false);
        }
        else if (sdata.obstacle)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %ld\n", tmnow - found_obstacle);
        }
        was_obstacle = sdata.obstacle;

        log_data( sdata, idata);

        cvShowImage( "camera", tmp_frame );

        cvReleaseImage( &tmp_frame );

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    sbot.stop();
    imu.stop();
    
    cvReleaseCapture( &capture );
    cvDestroyWindow( "camera" );
    cvDestroyWindow( "debug" );

    fclose(mainLog);

    return 0;
}

