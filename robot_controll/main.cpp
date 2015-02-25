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
#include <unistd.h>
#include <string.h>
#include <opencv/highgui.h>
#include <sys/time.h>
/*
//#############################
#include "VisionBase.h"
#include "VisionRegionReduced.h"
#include "VisionRegion.h"
#include "VisionHistogram.h"
*/

#include "ContextHistogram.h"
#include "ContextProbableRectangle.h"
#include "ModKMeans.h"
#include "VisionBaseNew.h"



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

//VisionHistogram nn(320, 240, 6, 6, 12);  //(720, 576, 6, 6, 12);
//VisionBase nn(320, 240, 4, 4);
//VisionBase nn(320, 240, 6, 6);
//EvalDireciton ed( nn.image_width*0.4, nn.image_height*0.7, 10, nn.image_width, nn.image_height );
//#############################
VisionContext* con = new VisionContext();//new VisionContext(); je prazdny kontext
VisionModifier* mod = new VisionModifier(); //new VisionModifier(); je prazdny modifier
vector<int> vmod;
vector<int> vcon;
VisionBase nn(5, 5, 5, 5, 1, 1,con,vcon,mod,vmod);

EvalDireciton ed( (nn.out_width)*0.4, (nn.out_height)*0.7, 10, nn.out_width, nn.out_height);
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
        if(start_time > time(NULL)){
            if(x>(locwin_width-150) && x<(locwin_width-110) ){
                if((y) > (locwin_map_height+25) && (y)<(locwin_map_height+50)){
                    start_time -=60;
                }
                else if((y) > (locwin_map_height) && (y)<(locwin_map_height+25)){
                    start_time +=60;
                }
            }
        }
    }
}
CvFont font;
CvFont fontBig;
void add_debug_to_image(IplImage** img,int top_margin,int maxwidth,SbotData sdat){
    //palceholder potom sa tam nahadzu senzory
    int senzor1 = sdat.distRL;
    int senzor2 = sdat.distFL;
    int senzor3 = sdat.distM;
    int senzor4 = sdat.distFR;
    int senzor5 = sdat.distRR;
    
    vector<int> se;
    se.push_back(senzor1);//LBocny
    se.push_back(senzor2);//Lavy Predny
    se.push_back(senzor3);//UUUUUplnePredny
    se.push_back(senzor4);//PravyPredny
    se.push_back(senzor5);//PBocny
    stringstream diststr;
    int trs = 80;//treshold of obstacle
    int maxim = 200;//max of sensor
    for(int i = 0; i<se.size();i++){
        //draw rectangle
        cvRectangle(*img,cvPoint(235+(i*15), top_margin + 45 -( 26-13*abs(i - 2)) ),cvPoint(255+(i*15), top_margin + 35 -( 26-13*abs(i - 2)) ),cvScalar(0, 1*( (double)max(se[i]-trs,0)/(maxim-trs) ), 1*(1- (double)max(se[i]-trs,0)/(maxim-trs) )) ,-1);
        
        diststr.str("");
        diststr << se[i];
        cvPutText(*img,diststr.str().c_str(),cvPoint(237+(i*15),top_margin + 43 -( 26-13*abs(i - 2))),&font,cvScalar(0,0,0));  
    }
    //curtime and start time
    time_t t1 = time(NULL);
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
        printf("data > %d %d %d %d %d %d %d %d %d %d %d %f \n", sdata.lstep, sdata.rstep, sdata.lspeed,
           sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR, kmtotarget*1000);
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
    start_time = time(NULL) +5*60;
    
    cvInitFont(&fontBig, CV_FONT_HERSHEY_SIMPLEX, 0.6, 0.6);
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3);
    
    char logname[64];
    sprintf(logname, "../logs/%ld.log", t);
    mainLog = fopen( logname, "w");
    if( mainLog == NULL ){
        printf("log cannot be created.\n");
        exit(-1);
    }

    IplImage* empty_frame = cvCreateImage( cvSize( camera_w, camera_h ), 8, 3);

    cvNamedWindow( "camera", CV_WINDOW_AUTOSIZE );
    //cvNamedWindow( "path", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "path", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );//Win.normal dovoluje resize lebo out je drobny a obrazok sa stretchne, keepratio je obvious
    cvNamedWindow( "localization", CV_WINDOW_AUTOSIZE );

    cvShowImage( "camera", empty_frame );
    cvShowImage( "path", empty_frame );
    cvShowImage( "localization", empty_frame );

    cvMoveWindow( "camera", 1000, 30 );
    cvMoveWindow( "path", 1000, 370 );
    cvMoveWindow( "localization", 30, 30 );

    LocalizationAndPlaning loc(800,400);
    locwin_width = loc.guiMapWidth;
    locwin_map_height = loc.guiMapHeight;
    //loc.readMap( "../maps/wien.osm" );
    //loc.readMap( (char *)"../maps/stromovka.osm" );
    //loc.readMap( (char *)"../maps/lodz.osm" );
    //loc.readMap( (char *)"../maps/homologacie_fei.osm" );
    loc.readMap( (char *)"../maps/botanicka.osm" );
    //loc.readMap( (char *)"../maps/borsky2.osm" );
    
    loc.readDestination( (char *)"../destination.txt");
    cvSetMouseCallback( "localization", loc_mouse_callback, NULL );


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
    //nn.load("../plzen4.net");//tiez mozno je ale az moc preuceno
    //nn.load("../555511.net");
    nn.load("../555511.net");
    
    CvMat* predicted_data;

    autonomy = true;


    int frame_counter = 0;

    bool was_obstacle = false;
    long found_obstacle;
    int finishcount = 0;
    int setstart = 0; //ci je setnuta start pozicija
    Ll startpos;
    int finnumber = 1;
    double lft = 0;
    double rgh = 0;
    int bakdir = 0;
    
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

        IplImage* tmp_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);
        IplImage* rgb_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);
        cvResize( frame, tmp_frame );
        cvResize( frame, rgb_frame );
	cvCvtColor(tmp_frame, tmp_frame, CV_BGR2Lab);
        //cvFlip( tmp_frame, tmp_frame, 0);
        //cvFlip( tmp_frame, tmp_frame, 1);
        
        SbotData sdata = sbot.getData();
        ImuData idata = imu.getData();
        Ll gdata = gps.getData();
        GpsAngles a = loc.update( gdata);
        //starting timer
        while( time(NULL)< start_time ){
            //printf("%d seconds to start\n", (int)(start_time - time(NULL)) );
            
            localizationFrame = loc.getGui();
            add_debug_to_image(&localizationFrame,loc.guiMapHeight,loc.guiDebugWidth,sdata);
            cvShowImage( "localization", localizationFrame );
            cvReleaseImage( &localizationFrame );

            char c = cvWaitKey(33);
            //sleep(1);
        }

        predicted_data = nn.predict( tmp_frame );
        //predicted_data = nn.nn_random_predict( tmp_frame, 1000 );

        //
        

        if(setstart == 0){
            setstart = 1;
            startpos = gdata;
        }
        
        if (sdata.obstacle && (!was_obstacle))
        {
            time_t tobs = time(NULL);
            found_obstacle = tobs;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
        }
        time_t tmnow = time(NULL);
        if ((sdata.obstacle) && (tmnow - found_obstacle > 20))
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
            found_obstacle = found_obstacle +30;
            // cuvat alebo aspon tocit!!
            lft = ed.eval( predicted_data,0 );
            rgh = ed.eval( predicted_data,ed.dir_count );
            
            if(lft>rgh){
                bakdir = -40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LEFT\n");
            }
            else{
                bakdir = 40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RIGHT\n");
            }
            sbot.ignoreObstacle(true);
            sbot.setDirection( 0 );
            sbot.setSpeed( -3 );
            sleep(4);
            sbot.setDirection( bakdir );
            sbot.setSpeed( 2 );
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

        

		//TODO ked 10sec stoji ze je v cieli vypni ho uz(ak je uspesne to zastavenie)
        if(a.map ==DBL_MAX){
            finishcount++;
        }
        else{
            finishcount = 0;
        }
        if(finishcount>60){
            if(finnumber == 2){
                printf("FINISH CONFIRMED with %d readings , TURNING OFF\n",finishcount);
                break;
            }
            else{
                for(int ghe = 0 ; ghe <7; ghe++)
                        printf("FINISH REACHED GOING BACK TO START\n");
                //TODO set next dest
                loc.setDestination(startpos);
                finnumber = 2;
            }
        }
        log_data( sdata, gdata, idata, a.map, a.dstToFin);

        int display_direction = coor.move(predicted_data, &sbot, a.map, idata.xAngle, &ed );

        //draw result
        int sizeC = tmp_frame->width / ed.dir_count;
        
        //cvLine( predicted_data, cvPoint( sizeC*b, (ed.frame_h-ed.triangle_h) ), cvPoint( ed.frame_w/2, ed.frame_h ), cvScalar( 0, 255 ) );
        //##################################################
        //cvLine( tmp_frame, cvPoint( sizeC*display_direction, (ed.frame_h-ed.triangle_h) ), cvPoint( ed.frame_w/2, ed.frame_h ), cvScalar( 0, 0,255 ), 5);
        //TODO aj tu tie stepx ponicit nech je to v evale a nie v main
        cvLine( rgb_frame, cvPoint( sizeC*(display_direction), (rgb_frame->height-ed.triangle_h*nn.step_y) ), cvPoint( rgb_frame->width/2, rgb_frame->height ), cvScalar( 0, 0,255 ), 5);

        localizationFrame = loc.getGui();
        add_debug_to_image(&localizationFrame,loc.guiMapHeight,loc.guiDebugWidth,sdata);
        cvShowImage( "camera", rgb_frame );
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