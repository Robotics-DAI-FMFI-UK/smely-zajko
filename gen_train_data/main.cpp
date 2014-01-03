
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <cv.h>
#include <ml.h>
#include <highgui.h>

#include "VisionBase.h"
#include "VisionRegionReduced.h"
#include "VisionRegion.h"
#include "VisionHistogram.h"


#include <qt4/QtGui/QFileDialog>
#include <qt4/QtCore/QString>
#include <qt4/QtGui/QApplication>
#include <qt4/QtCore/QDir>
#include <qt4/QtCore/QFile>
#include <qt4/QtCore/QStringList>
#include <qt4/QtCore/QLocale>

#include <vector>

using namespace std;

VisionHistogram nn(720, 576, 6, 6, 12);


CvMat* data;

CvMat* predicted_data = NULL;
IplImage* frame;


int frame_width = nn.image_width, frame_height = nn.image_height;
int sq_w = 1;
int sq_h = 1;

QDir dir;
QString path;
QString default_xml_path;

int file_index = 0;
QStringList sl;

bool is_nn_loaded = false;
bool show = true;
bool show_predicted = false;
//bool equalize_hist = false;

bool to_fill = false;

int last_mouse_x = -1;
int last_mouse_y = -1;

void my_mouse_callback(int event, int x, int y, int flags, void* param){
    int i = x/sq_w;
    int j = y/sq_h;

    if( event == CV_EVENT_LBUTTONDOWN || ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)  ) ){
        if( to_fill){
            cvFloodFill( data, cvPoint(i, j), cvScalar(1) );
            to_fill =false;
        }else{
            if(last_mouse_x!=-1){
                cvLine( data, cvPoint(i, j), cvPoint(last_mouse_x, last_mouse_y), cvScalar(1),3 );
            }else{
                //cvSet2D( data, j, i, cvScalar(1) );
                cvCircle( data, cvPoint(i, j), 1,cvScalar(1), 2);

            }
        }
    }else if( event == CV_EVENT_RBUTTONDOWN  || ( event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_RBUTTON)  ) ){
        cvCircle( data, cvPoint(i, j), 1, cvScalar(0), 10 );
    }

    last_mouse_x = i;
    last_mouse_y = j;
}

void draw_gui( IplImage* frame, CvMat *data){

    for(int i=0; i<data->width; i++){
        for(int j=0; j<data->height; j++){
            CvScalar s = cvGet2D(data,j,i);
            if( s.val[0]==1 ){
                CvPoint p1 = cvPoint((i* sq_w), (j* sq_h));
                cvSet2D( frame, p1.y, p1.x, cvScalar(0,0,255) );
                //cvCircle( frame, p1, 1,cvScalar(0,0,255), 2);
            }
        }
    }
}

void draw_gui2( IplImage* frame, CvMat *data){

    for(int i=0; i<data->width; i++){
        for(int j=0; j<data->height; j++){
            CvScalar s = cvGet2D(data,j,i);
            CvPoint p1 = cvPoint((i* sq_w), (j* sq_h));
            cvCircle( frame, p1, 1,cvScalar(255*s.val[0] ,255*s.val[0], 255*s.val[0]), 2 );
        }
    }
}

void load_data(QString* file = NULL){
   
    if(data != NULL){
        cvReleaseMat( &data );
    }


    CvMat* tmp;

    if(file == NULL){
        tmp = (CvMat*) cvLoad( (const char *)default_xml_path.toAscii() );
        printf("loaded data: %s\n", (const char *)default_xml_path.toAscii() );
    }else{
        tmp = (CvMat*) cvLoad( (const char *)file->toAscii() );
        printf("loaded data: %s\n", (const char *)file->toAscii() );
    }

    data = cvCreateMat( nn.image_height, nn.image_width, CV_8U);
    cvResize(tmp, data);

}

void save_data(QString* file = NULL){
    if(file == NULL){
        cvSave( (const char *)default_xml_path.toAscii(), data );
        printf("saved data: %s\n", (const char *)default_xml_path.toAscii() );
    }else{
        cvSave( (const char *)file->toAscii(), data );
        printf("saved data: %s\n", (const char *)file->toAscii() );
    }
}

void save_frame( IplImage*  frame){

    QFile file;
    int i = 0;
    do{
        file.setFileName( path +  QDir::separator() + "scr" + QString::number(i) + ".jpg" );
        i++;
    }while( file.exists() );

    cvSaveImage( (const char *)file.fileName().toAscii(), frame );
    printf("saved data: %s\n", (const char *)file.fileName().toAscii() );
}

void load(){
    QString tmp;
    tmp += path;
    tmp += dir.separator();
    tmp += sl.at( file_index );

    if(frame != NULL){
        cvReleaseImage( &frame );
    }

    IplImage* tmp_frame = cvLoadImage( (const char *)tmp.toAscii() );
    frame = cvCreateImage(
            cvSize(nn.image_width, nn.image_height ),//cvSize(frame->width, frame->height ),
            tmp_frame->depth,
            tmp_frame->nChannels
        );
    cvResize( tmp_frame , frame );
    cvReleaseImage(&tmp_frame);

    printf("loaded image: %s\n", (const char *)tmp.toAscii() );

    int i = tmp.lastIndexOf(QChar('.'));
    default_xml_path = tmp.left(i);
    default_xml_path += ".xml";
    
    QFile f(default_xml_path);
    if( f.exists() ){
        load_data();
    }else{
        if(data == NULL){
            data = cvCreateMat( nn.image_height, nn.image_width, CV_8U);
            cvSetIdentity( data, cvScalar(0) );
        }
        cvSetIdentity( data, cvScalar(0) );
    }
}

int main()
{
    //cout << "debug 01";
    //QApplication *app = new QApplication(NULL,NULL);

    QApplication *app = new QApplication(NULL,NULL);

    setlocale(LC_ALL, "C");

    path = QFileDialog::getExistingDirectory();
    dir = QDir();
    dir.setPath(path);

    QStringList filters;
    filters << "*.jpg" << "*.JPG" << "*.png" << "*.PNG";
    dir.setNameFilters(filters);
    sl = dir.entryList();

    cvNamedWindow( "gen_train_data", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "debug", CV_WINDOW_AUTOSIZE );

    cvSetMouseCallback( "gen_train_data", my_mouse_callback, NULL );

    load();

    IplImage* tmp_frame;


    while(1) {
        if( !frame ) break;

        tmp_frame = cvCreateImage(
            cvSize(nn.image_width, nn.image_height ),//cvSize(frame->width, frame->height ),
            frame->depth,
            frame->nChannels
        );
         cvCopy( frame, tmp_frame );

        if(show){
            if(!show_predicted){
                draw_gui( tmp_frame, data);
            }else{
                draw_gui2( tmp_frame , predicted_data );
            }
        }

        cvShowImage( "gen_train_data", tmp_frame);

        CvMat* tmp_data = cvCreateMat( data->rows, data->cols, data->type );
        cvScale( data, tmp_data, 255 );
        cvShowImage( "debug", tmp_data);
        cvReleaseMat(&tmp_data);

        char c = cvWaitKey(100);
        if( c == 27 ) break; //esc
        if( c == 's' ){
            QString str = QFileDialog::getSaveFileName();

            save_data( &str);
        }
        if( c == 'l' ){
            QString str = QFileDialog::getOpenFileName();

            load_data( &str );
        }
        if( c == ' ' ){ //next + autoload - dalsi obrazok
            show_predicted = false;

            file_index++;
            if( file_index>=sl.size()  ){
                file_index=0;
            }

            load();
        }
        if( c == 8 ){ //previous + autoload
            show_predicted = false;

            file_index--;
            if( file_index<0){
                file_index=sl.size()-1;
            }

            load();
        }
        if( c == 10 ){ //enter default_save - ulozi
            save_data();
        }
        if( c == 'r' ){ //screenshot
            save_frame( frame );
        }
        if( c == 'f' ){ //vypln
            to_fill = true;
        }
        if( c == 'v' ){ //viditelnost grafiky
            show = !show;
        }
        if( c == 'p' ){ //predict
            //printf("#");
            if(!is_nn_loaded){
                QString str = QFileDialog::getOpenFileName();
                nn.load( (const char *)str.toAscii() );
                printf("loaded NN file: %s\n", (const char *)str.toAscii() );
                is_nn_loaded = true;
            }
            printf( "1 \n");
            IplImage* tmp = cvCreateImage( cvSize(nn.image_width, nn.image_height), frame->depth, frame->nChannels);

            cvResize( frame, tmp );
            //equalize_hist( tmp );
            predicted_data = nn.predict( tmp );

            cvReleaseImage( &tmp );

            show_predicted = true;
            //double e = nn_get_error( data, predicted_data );

            //printf("showing predicted data, error: %f\n", e );
            printf( "2 \n");
        }
        cvReleaseImage( &tmp_frame );

        usleep( 1000 );
    }
    
    if(is_nn_loaded){
        nn.close();
        cvReleaseMat( &predicted_data );
    }

    cvReleaseImage( &frame );
    cvReleaseMat( &data );
    cvDestroyWindow( "gen_train_data" );
    
//    return 0;

}
