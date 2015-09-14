#include <cv.h>
#include <highgui.h>

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <opencv/highgui.h>
#include <sys/time.h>



#include "Subroutines.h"
#include "SensorManagement.h"
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
bool START_TIMER = 0;
const int camera_w = 320;
const int camera_h = 240;

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

Point pixel;
int clicked;
time_t start_time;
int locwin_width;
int locwin_map_height;
bool debugging=false;
void loc_mouse_callback(int event, int x, int y, int flags, void* param) {
    if(debugging)
        if (event == CV_EVENT_LBUTTONDOWN || (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))) {
            pixel.x = x;
            pixel.y = locwin_map_height-y;
            clicked = 1;
        }
    if (event == CV_EVENT_LBUTTONDOWN){
        //allow start time change if we did not start yet
        if(start_time > time(0)){
            if(x>(locwin_width-150) && x<(locwin_width-110) ){
                if((y) > (locwin_map_height+25) && (y)<(locwin_map_height+50)){
                    start_time -= 60;
                }
                else if((y) > (locwin_map_height) && (y)<(locwin_map_height+25)){
                    start_time += 60;
                }
            }
            
            if(x>(locwin_width-110) && x<(locwin_width-20) ){
                if((y) > (locwin_map_height+25) && (y)<(locwin_map_height+50)){
                    start_time -= 5;
                }
                else if((y) > (locwin_map_height) && (y)<(locwin_map_height+25)){
                    start_time += 5;
                }
            }            
        }
    }
}
CvFont font;
CvFont fontBig;
void add_debug_to_image(IplImage** img,int top_margin,int maxwidth,SbotData sdat){
    vector<int> se;
    se.push_back(sdat.distRL);//LBocny
    se.push_back(sdat.distFL);//Lavy Predny
    se.push_back(sdat.distM);//UUUUUplnePredny
    se.push_back(sdat.distFR);//PravyPredny
    se.push_back(sdat.distRR);//PBocny
    stringstream diststr;
    int trs = 80;//treshold of obstacle
    int maxim = 200;//max of sensor
    for(int i = 0; i<se.size();i++){
        //draw rectangle
        cvRectangle(*img, cvPoint(235+(i*15), top_margin + 45 -( 26-13*abs(i - 2)) ), cvPoint(255+(i*15), top_margin + 35 -( 26-13*abs(i - 2)) ), cvScalar(0, 1*( (double)max(se[i]-trs,0)/(maxim-trs) ), 1*(1- (double)max(se[i]-trs,0)/(maxim-trs) )) ,-1);
        
        diststr.str("");
        diststr << se[i];
        cvPutText(*img,diststr.str().c_str(),cvPoint(237+(i*15),top_margin + 43 -( 26-13*abs(i - 2))),&font,cvScalar(0,0,0));  
    }
    //curtime and start time
    time_t t1 = time(0);
    struct tm  t; 
    localtime_r(&t1,&t);
    struct tm  t2;
    localtime_r(&start_time,&t2);
    char buffer[20];
    char buffer2[20];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &t);    
    cvPutText(*img,buffer,cvPoint(maxwidth-100,top_margin +20),&fontBig,cvScalar(0,0,0));  
    strftime(buffer2, sizeof(buffer2), "%H:%M:%S", &t2);
    cvPutText(*img,buffer2,cvPoint(maxwidth-100,top_margin + 45 ),&fontBig,cvScalar(0,0,0)); 
    //start time +- 'buttons'
    cvLine(*img,cvPoint(maxwidth - 150,top_margin),cvPoint(maxwidth - 150,top_margin+50),cvScalar(0,0,0));
    cvLine(*img,cvPoint(maxwidth - 110,top_margin),cvPoint(maxwidth - 110,top_margin+50),cvScalar(0,0,0));
    cvLine(*img,cvPoint(maxwidth - 150,top_margin+25),cvPoint(maxwidth,top_margin+25),cvScalar(0,0,0));
    diststr.str("+");
    cvPutText(*img,diststr.str().c_str(),cvPoint(maxwidth-137,top_margin +20),&fontBig,cvScalar(0,0,0)); 
    diststr.str("-");
    cvPutText(*img,diststr.str().c_str(),cvPoint(maxwidth-137,top_margin + 45 ),&fontBig,cvScalar(0,0,0)); 
}

void log_data(SbotData sdata, Ll gdata, ImuData idata, double mapAngle, double kmtotarget )
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
        printf("data > %d %d %d %d %d %d %d %d %d %d %d %f %3.3f\n", sdata.lstep, sdata.rstep, sdata.lspeed,
           sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR, kmtotarget*1000, idata.xAngle);
    }
}

/*
 * 
 */

int main(int argc, char** argv)
{
    IplImage* localizationFrame;

    setlocale(LC_ALL, "C");
    time_t t;
    time(&t);
    start_time = time(0) +5*60;
    
    cvInitFont(&fontBig, CV_FONT_HERSHEY_SIMPLEX, 0.6, 0.6);
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3);
	
    char logname[64];
    sprintf(logname, "../logs/%ld.log", t);
    mainLog = fopen( logname, "w");
    if( mainLog == 0 )
	{
        printf("log cannot be created.\n");
        exit(-1);
    }

    IplImage* empty_frame = cvCreateImage( cvSize( camera_w, camera_h ), 8, 3);

    cvNamedWindow( "camera", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "path", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );//Win.normal dovoluje resize lebo out je drobny a obrazok sa stretchne, keepratio je obvious
    cvNamedWindow( "localization", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "laser", CV_WINDOW_AUTOSIZE );
    
    cvShowImage( "camera", empty_frame );
    cvShowImage( "path", empty_frame );
    cvShowImage( "localization", empty_frame );
    cvShowImage( "laser", empty_frame );

    cvMoveWindow( "camera", 1000, 30 );
    cvMoveWindow( "laser", 30, 470 );
    cvMoveWindow( "path", 1000, 370 );
    cvMoveWindow( "localization", 30, 30 );
    cvSetMouseCallback( "localization", loc_mouse_callback, NULL );
	
	SensorManagement sm;
	sm.init();

    locwin_width = sm.loc->guiMapWidth;
    locwin_map_height = sm.loc->guiMapHeight;
    //sm.loc->readMap( "../maps/wien.osm" );
    //sm.loc->readMap( (char *)"../maps/stromovka.osm" );
    //sm.loc->readMap( (char *)"../maps/lodz.osm" );
    //sm.loc->readMap( (char .maps/homologacie_fei.osm" );
    //sm.loc->readMap( (char *)"../maps/botanicka.osm" );
    //sm.loc->readMap( (char *)"../maps/borsky2.osm" );
    sm.loc->readMap( (char *)"../maps/pisek.osm" );
    
    sm.loc->readDestination( (char *)"../destination.txt");
    
    Subroutines subroutine;
    subroutine.setup(sm.loc, sm.sbot);

    CvCapture* capture;

    if( START_CAM )
	{
        //init_video();
        printf("mplayer init video done.\n");
        capture = cvCaptureFromCAM( 1);  //(1);
    }else
	{ 
        //debug
        //capture = cvCreateFileCapture( "../../video/b23.avi");
        capture = cvCreateFileCapture( "../../stromovka-friday/236.jpg");
    }
     
    
	subroutine.testSbot();
    
    IplImage* frame;
    setlocale(LC_ALL, "C");
        
    autonomy = true;

    int frame_counter = 0;
    time_t photoTime = time(0) +5;
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
		
        IplImage* rgb_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);        
        cvResize( frame, rgb_frame );
	IplImage* tmp_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);
	cvFlip( rgb_frame, rgb_frame, 0);
        cvFlip( rgb_frame, rgb_frame, 1);
        cvCvtColor(rgb_frame, tmp_frame, CV_BGR2Lab);
        if(time(0) > photoTime)
        {
            //save jpeg img
            char str[100];
            sprintf(str, "../runPhotos/cameraimg%ld.jpg", photoTime);
            cvSaveImage(str, rgb_frame);
            
            //hokuyo log
            sprintf(str, "../runHokuyo/hokuLog%ld.txt", photoTime);
            sm.saveHokuyoLog(str);
            photoTime = time(0) + 5;
        }
        //starting timer
        while(time(0) < start_time){
            //printf("%d seconds to start\n", (int)(start_time - time(NULL)) );
            
            localizationFrame = sm.loc->getGui();
            add_debug_to_image(&localizationFrame,locwin_map_height, locwin_width, sm.sdata);
            cvShowImage( "localization", localizationFrame );
            cvReleaseImage( &localizationFrame );

            char c = cvWaitKey(33);
        }
                sm.update();
        
		sm.neuralPredict(tmp_frame);
        
        subroutine.manageObstacles(sm);
		
		if(subroutine.manageFinish(sm))
			break;
		
        log_data( sm.sdata, sm.gdata, sm.idata, sm.angles.map, sm.angles.dstToFin);

        int display_direction = sm.move();

        //draw result
        int sizeC = tmp_frame->width / sm.ed->dir_count;        
        cvLine( rgb_frame, cvPoint( sizeC*(display_direction), (rgb_frame->height - sm.ed->triangle_h*sm.nn->step_y) ), cvPoint( rgb_frame->width/2, rgb_frame->height ), cvScalar( 0, 0,255 ), 5);

        localizationFrame = sm.loc->getGui();
        add_debug_to_image(&localizationFrame,locwin_map_height, locwin_width, sm.sdata);
        cvShowImage( "camera", rgb_frame );
        cvShowImage( "laser", sm.getLaserFrame() );
        cvShowImage( "path", sm.predicted_data );
        cvShowImage( "localization", localizationFrame );
        
        
        // cvReleaseImage( &frame );
       
        cvReleaseImage( &rgb_frame );
        cvReleaseImage( &tmp_frame );
        cvReleaseImage( &localizationFrame );

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    sm.stop();
    cvReleaseCapture( &capture );
    cvDestroyWindow( "camera" );
    cvDestroyWindow( "laser" );
    cvDestroyWindow( "debug" );
    cvDestroyWindow( "localization" );

    fclose(mainLog);

    return 0;
}