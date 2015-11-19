#include "ImuThread.h"

#include <stdio.h>

#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <iosfwd>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <linux/unistd.h>

using namespace std;

bool ImuThread::end;
pthread_mutex_t ImuThread::m_read;
int ImuThread::imu = 0;
ImuData ImuThread::data;
char ImuThread::IMUPORT[32];

ImuThread::ImuThread() 
{
    end = true;
}

int ImuThread::validate(char* devName)
{
    if (!imuOpen(devName)) return 0;
    
    //validate
    char b[1024];

    for(int i=0; i<3; i++)
    {        
        usleep( 100000 );//100ms

        if (!readLine(b))
        {
            close( imu );
            return 0;
        }

        if( data.xAngle>=0 && data.xAngle<3600 &&
            data.yAngle>=-900 && data.yAngle<=900 &&
            data.zAngle>=-900 && data.zAngle<=900)
        {
            //printf( "%f %f %f\n", data.xAngle, data.yAngle, data.zAngle  );
            close(imu);
            return 1;            
        }
    }
    
    close( imu );
    return 0;
}

int ImuThread::imuOpen(char *filename)
{
    imu = open(filename, O_RDWR | O_NOCTTY | O_NONBLOCK );
    
    if(imu<0)
    {
        printf("imu not found at %s\n", filename);
        return 0;
    }

    struct termios oldtio,newtio;

    tcgetattr(imu,&oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD ; //B57600
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 char received */

    tcflush(imu, TCIFLUSH);
    tcsetattr(imu,TCSANOW,&newtio);
    
    return 1;
}

void ImuThread::init()
{
    if (end) return;
    if (!imuOpen(IMUPORT)) end = true;
}

void ImuThread::setDeviceName(char* dev_name)
{
    strcpy(IMUPORT, dev_name );
    end = false;
}

int ImuThread::readLine(char *b)
{
        int readptr = 0;
        int nread;
        do {
            if ((nread = read(imu, b + readptr, 1)) < 0)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) 
                {
                    usleep(25000);
                    continue;
                }
                else return 0;
            }
            readptr += nread;
        } while ((!end) && (b[readptr - 1] != '\n'));
        b[readptr] = 0;
        return 1;
}

void* ImuThread::mainLoop(void*)
{
   char b[1024];

   FILE* inlog;
   inlog = fopen( "imu-in.log", "a" );

    while (!end)
    {
        //reinit();
        usleep( 100000 );//100ms

        if (!readLine(b))
        {
            perror("Error reading from compass");
            end = true;
            break;
        }

        //printf( "%s\n", b );

        //fprintf( inlog, "%ld %s", time(NULL), b );
        time_t t = time(0);
        fprintf(inlog, "%s|%s", ctime(&t) ,b);
        fflush(inlog);

        int xAngle, yAngle, zAngle;
        sscanf(b, "%d %d %d", &xAngle, &yAngle, &zAngle);

        pthread_mutex_lock( &m_read );
        data.xAngle = xAngle + COMPASS_ALIGNMENT;
        data.yAngle = yAngle;
        data.zAngle = zAngle;
        pthread_mutex_unlock( &m_read);
    }

   close( imu );
   fclose( inlog );
}

void ImuThread::run(){

    if (end) return;

    pthread_mutex_init( &m_read, NULL );
    pthread_create( &t, NULL, mainLoop, NULL );
}

void ImuThread::stop()
{
    if(!end)
    {
        end = true;
        usleep(250000);
        if( pthread_join( t, NULL )!=0)
            printf("imu thread join error");        
    }
}

ImuData ImuThread::getData()
{
    ImuData result;
    if (end) { return result; }
    pthread_mutex_lock( &m_read);
    result = data;
    pthread_mutex_unlock( &m_read);
    return result;
}
