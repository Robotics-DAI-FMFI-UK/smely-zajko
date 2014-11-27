#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "DataTypes.h"
#include "LocalizationAndPlaning.h"

Point pixel;
int clicked;
int ydiff;
CvFont font;
CvFont fontBig;
time_t start_time;
int locwin_width;
int locwin_map_height;
bool debugging = true;//loc debug set to false to ignore map input

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

void add_debug_to_image(IplImage** img,int top_margin,int maxwidth){
    //palceholder potom sa tam nahadzu senzory
    int senzor1 = 200;
    int senzor2 = 180;
    int senzor3 = 150;
    int senzor4 = 80;
    int senzor5 = 100;
    
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

void timer(){
    time_t t = time(NULL);

    printf("current time: %s \n", ctime( &t) );
    time_t t2 = t + 5*60;
    printf("current time+5min: %s \n", ctime( &t2) );
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


int main(int argc, char** argv) {
    
    printf("start\n");
    start_time = time(NULL) +5*60;//5min def
    
    cvInitFont(&fontBig, CV_FONT_HERSHEY_SIMPLEX, 0.6, 0.6);
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3);
    IplImage* localizationFrame;
    pixel.x = 0;
    pixel.y = 0;
    cvNamedWindow("localization", CV_WINDOW_FREERATIO);
    cvMoveWindow("localization", 30, 30);

    LocalizationAndPlaning loc(800, 600);
    locwin_width = loc.guiMapWidth;
    locwin_map_height = loc.guiMapHeight;
    loc.readMap((char *) "./botanicka.osm");
    loc.readDestination((char *) "./destination.txt");
    cvSetMouseCallback("localization", loc_mouse_callback, NULL);
    //timer();
    while (1) {
        
        //printf("kremes\n");
        Ll gdata = loc.reverse(pixel);
        GpsAngles a = loc.update(gdata);


        localizationFrame = loc.getGui();
        add_debug_to_image(&localizationFrame,loc.guiMapHeight,loc.guiDebugWidth);
        
        cvShowImage("localization", localizationFrame);
        cvWaitKey(60);
        cvReleaseImage(&localizationFrame);

    }
    
    cvDestroyWindow("localization");

    return 0;
}