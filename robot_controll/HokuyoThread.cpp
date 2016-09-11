#include "HokuyoThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sstream>
#include <iostream>
#include <iosfwd>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <math.h>

#include <linux/unistd.h>

#include <cmath>

#include <cv.h>
#include <highgui.h>

#define BUFFER_SIZE 5000

using namespace std;

bool HokuyoThread::end;
pthread_mutex_t HokuyoThread::m_read;
int *HokuyoThread::data;
int HokuyoThread::sockfd;
int HokuyoThread::guiWidth;
int HokuyoThread::guiHeight;
IplImage* HokuyoThread::result;

HokuyoThread::HokuyoThread() {
    data = new int[RANGE_DATA_COUNT];
    end = false;
    guiWidth = 320;
    guiHeight = 240;    
}

void HokuyoThread::init(){

   struct sockaddr_in serv_addr;
   struct hostent *server;
   result = cvCreateImage(cvSize(guiWidth, guiHeight), 32, 3);
   
    if (end) {
        return;
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
   {
      perror("ERROR opening Hokuyo socket");
      end = true;
      return;
   }
    
   struct sockaddr_in remoteaddr;
   remoteaddr.sin_family = AF_INET;
   remoteaddr.sin_addr.s_addr = inet_addr(HOKUYO_ADDR);
   remoteaddr.sin_port = htons(HOKUYO_PORT);
    
   if (connect(sockfd, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) < 0)
   {
      perror("ERROR connecting Hokuyo");
      end = true;
      return;
   }

   printf("Hokuyo connected\n");
}

void* HokuyoThread::mainLoop(void*){

   FILE* inlog;
   inlog = fopen( "../logs/hokuyo-in.log", "a" );
   char *start_measurement = "BM\n";
   char *request_measurement = "GD0000108000\n";
   unsigned char readbuf[BUFFER_SIZE];
      
   //printf("HOK1\n");
   if (write(sockfd, start_measurement, strlen(start_measurement)) < 0)
   {
       perror("Error writing to Hokuyo\n");
       end = true;
   }
   usleep(250000);
   
   unsigned char x[2];
   int cnt = 0;
   do {
     if (read(sockfd, x + ((cnt++) % 2), 1) < 0)
     {
         perror("Error reading response from Hokuyo");
         end = true;
         break;
     }
   } while ((x[0] != 10) || (x[1] != 10));
   
   //printf("HOK2\n");
   while (!end)
   {        
       if (write(sockfd, request_measurement, strlen(request_measurement)) < 0)
       {
           perror("Error writing to Hokuyo\n");
           end = true;
       }       
       //printf("HOK3\n");
       
       int readptr = 0;
       do {
         int nread = read(sockfd, readbuf + readptr, BUFFER_SIZE - readptr);
         if (nread < 0)
         {
             perror("Problem reading from Hokuyo");
             end = 0;
             break;
         }
         readptr += nread;
         //printf("nread=%d, readptr=%d readbuf[readptr - 1]=%d, readbuf[readptr - 2]=%d\n", nread, readptr, readbuf[readptr - 1], readbuf[readptr - 2]);
         if (readptr < 2) continue;
       } while ((readbuf[readptr - 1] != 10) || (readbuf[readptr - 2] != 10));
       //printf("HOK4\n");
       
       int searchptr = 0;
       for (int i = 0; i < 3; i++)
       {
           while ((readbuf[searchptr] != 10) && (searchptr < readptr)) searchptr++;
           searchptr++;
       }
       //printf("HOK5\n");
       
       if (readptr - searchptr != 103 + RANGE_DATA_COUNT * 3)
       {
           printf("Hokuyo returned packet of unexpected size, I will ignore it size=%d\n", readptr - searchptr);
           continue;
       }
       
       int beam_index = RANGE_DATA_COUNT - 1;
       readptr = searchptr;
       pthread_mutex_lock( &m_read);
       //printf("HOK6\n");
       int counter = 0;
       while (beam_index >= 0)
       {
           //printf("%d (%d %d %d)", counter++, (int)readbuf[searchptr], (int)readbuf[searchptr + 1], (int)readbuf[searchptr + 2]);
           int pos = (searchptr - readptr) % 66;
           if (pos == 62)
           {
               data[beam_index] = ((readbuf[searchptr] - 0x30) << 12) | ((readbuf[searchptr + 1] - 0x30) << 6) | (readbuf[searchptr + 4] - 0x30);
               searchptr += 5;             
           }
           else if (pos == 63)
           {
               data[beam_index] = ((readbuf[searchptr] - 0x30) << 12) | ((readbuf[searchptr + 3] - 0x30) << 6) | (readbuf[searchptr + 4] - 0x30);
               searchptr += 5;             
           }
           else
           {
               if (pos == 64) searchptr += 2;
               data[beam_index] = ((((int)readbuf[searchptr]) - 0x30) << 12) | ((((int)readbuf[searchptr + 1]) - 0x30) << 6) | (((int)readbuf[searchptr + 2]) - 0x30);
               searchptr += 3;
           }
           //printf("  => %d; ", data[beam_index]);
           beam_index--;
       }
       //printf("HOK7\n");
       
        pthread_mutex_unlock( &m_read);
        
        //printf("HOKUYO: %d %d %d %d %d\n", data[550], data[535], data[540], data[545], data[530]);
        usleep(25000);
   }

   fclose(inlog);
}

void HokuyoThread::run(){

    if (end) return;

    pthread_mutex_init( &m_read, NULL );

    pthread_create( &t, NULL, mainLoop, NULL );
}

void HokuyoThread::stop(){
    if(!end)
    {
        end = true;
        usleep(300000);

        if( pthread_join( t, NULL )!=0){
            printf("hokuyo thread join error");
        }
    }
}

void HokuyoThread::getData(int *buffer){

    if (end) {
        return;
    }

    pthread_mutex_lock( &m_read);
    memcpy(buffer, data, sizeof(int) * RANGE_DATA_COUNT);
    pthread_mutex_unlock( &m_read);
}


IplImage* HokuyoThread::getGuiFrame(int* dataArr){

    cvSet(result, CV_RGB(255,255,255));
    int x,y;
    double brkAngle = -135 / 180.0 * M_PI;
    double deltaAngle = 0.25 / 180 * M_PI;
    for(int i = 0; i < RANGE_DATA_COUNT; i++)
    {
        x = (int)(- dataArr[i] * sin(brkAngle + i * deltaAngle)/35  +guiWidth/2);
        y = (int)(dataArr[i] * cos(brkAngle + i * deltaAngle)/35 + guiHeight/2);
        cvLine(result, cvPoint(guiWidth / 2, guiHeight / 2), cvPoint(x, guiHeight-y), cvScalar(0.4, 0.4, 0.4));
    }
    for(int i = 0; i < RANGE_DATA_COUNT; i++)
    {
        x = (int)(- dataArr[i] * sin(brkAngle + i * deltaAngle)/35  +guiWidth/2);
        y = (int)(dataArr[i] * cos(brkAngle + i * deltaAngle)/35 + guiHeight/2);
        cvCircle(result, cvPoint(x, guiHeight-y), 2, cvScalar(0.6, 0.8, 0),-1);
    }
    return result;
}