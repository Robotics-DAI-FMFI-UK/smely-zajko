#include "GpsThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <iosfwd>
#include <iostream>
#include <sstream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <linux/unistd.h>

#include <cmath>

#include <cv.h>
#include <highgui.h>

using namespace std;

bool GpsThread::end;
pthread_mutex_t GpsThread::m_read;
Ll GpsThread::data;
string GpsThread::buffer;

int GpsThread::gps;
static int online_mode;

GpsThread::GpsThread(int is_online_mode) { end = true; online_mode = is_online_mode; }

void GpsThread::setDeviceName(char* dev_name) {
    strcpy(device_name, dev_name);
    end = false;
}

int GpsThread::validate(char* dev_name) {
    char command[50];

    sprintf(command, "gpsctl -T 10 -f -n %s 2>&1", dev_name);
    return 1;

    FILE* gpsctl;

    if (!(gpsctl = (FILE*)popen(command, "r"))) {
        // If fpipe is NULL
        perror("Problems with gpsctl pipe");
        // exit(1);
    }

    char b[1024];
    string s("ststrgsrth");

    while (fgets(b, 1024, gpsctl) && !feof(gpsctl)) {

        b[1023] = 0;
        s += b;

        // printf("%s", b);

        int pos;
        pos = s.find("identified");

        if (pos != string::npos) {
            // printf("@>%s\n %d\n", s.c_str(), pos);

            pclose(gpsctl);
            return 1;
        }
    }

    pclose(gpsctl);
    return 0;
}

void GpsThread::init() {

    if (end) {
        return;
    }
    
    if (!online_mode) return;

    struct termios oldtio, newtio;

    gps = open(device_name, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (gps < 0) {
        // printf("imu not found at %s\n", devName);
        return;
    }

    tcgetattr(gps, &oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B4800 | CS8 | CLOCAL | CREAD; // B57600
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
    newtio.c_cc[VMIN] = 1;  /* blocking read until 1 char received */

    tcflush(gps, TCIFLUSH);
    tcsetattr(gps, TCSANOW, &newtio);
}

void* GpsThread::mainLoop(void*) {

    cvNamedWindow("localization", CV_WINDOW_AUTOSIZE);

    char b[1024];

    char b2[1024];
    int bufp = 0;

    FILE* inlog;
    inlog = fopen("../logs/gps-in.log", "a");
    if (online_mode)
        do {
            if (!read(gps, b, 1))
                break;
        } while (b[0] != '\n');
    while (!end) {
        int nread;
        if (online_mode)
        {
            if (!(nread = read(gps, b + bufp, 1)))
                break;
        } else
        {
            //TODO: read from previously saved logfile
            strcpy(b, "$GPGGA,183308.000,4809.0886,N,01704.3730,E,1,10,0.9,179.9,M,42.7,M,,0000*54\n");
            bufp = strlen(b) - 1;
            sleep(1);
        }
        if (nread < 0) {
            usleep(3000);
            continue;
        }
        if (b[bufp] == '\n') {
            b[bufp] = '\0';
            bufp = 0;
            strncpy(b2, b, 1023);
            b2[1023] = '\0';
            parseGpsLine(b);

            time_t t = time(NULL);
            fprintf(inlog, "%s|%s\n", ctime(&t), b2);
            fflush(inlog);
        } else if (bufp < 1023) {
            //           cout << b[bufp];
            bufp++;
        } else
            bufp = 0;
    }
    /*
       while ( read(gps, b, 1024)  && !end ){

            pthread_mutex_lock( &m_read );

            buffer += b;

            //fprintf( inlog, "%ld %s", time(NULL), b );
            time_t t = time(NULL);
            fprintf(inlog, "%s|%s", ctime(&t) ,b);
            fflush(inlog);

            int pos;
            pos = buffer.find('\n');

            if( pos!=string::npos ){
                istringstream is(buffer);
                string line;
                is >> line;


                //printf("> %s \n\n", line.c_str());

                parseGpsLine(line);
                //buffer.erase(0, pos+1);
                buffer.clear();
            }

            pthread_mutex_unlock( &m_read);

            usleep( 100000 );//100ms
        } */

    if (online_mode) close(gps);
    fclose(inlog);
}

void GpsThread::run() {
    if (end)
        return;
    pthread_mutex_init(&m_read, NULL);
    pthread_create(&t, NULL, mainLoop, NULL);
}

void GpsThread::stop() {
    if (!end) {
        end = true;

        if (pthread_join(t, NULL) != 0) {
            printf("gpd thread join error");
        }
    }
}

Ll GpsThread::getData() {

    Ll result;

    if (end) {
        return result;
    }

    pthread_mutex_lock(&m_read);

    result = data;

    pthread_mutex_unlock(&m_read);

    return result;
}

void GpsThread::parseGpsLine(char* s) {
    // istringstream iss( s, istringstream::in);

    // string s2;

    // iss.width( 6);
    // iss >> s2;

    Ll result;
    result.latitude = 0;
    result.longitude = 0;

    // printf("gps> %s", s.c_str()c_str);
    // cout << "GPS: " << s << '\n';

    if (strncmp(s, "$GPGGA", 6) == 0) {
        long l = 0;
        double t;
        Ll p;

        char* comma = strchr(s + 7, ',');
        *comma = '\0';
        sscanf(s + 7, "%lf", &t);
        l = (long)t;
        char* comma2 = strchr(comma + 1, ',');
        *comma2 = '\0';
        if (!sscanf(comma + 1, "%lf", &p.latitude))
            p.latitude = 0.0;
        comma = strchr(comma2 + 3, ',');
        *comma = '\0';
        if (!sscanf(comma2 + 3, "%lf", &p.longitude))
            p.latitude = 0.0;

        if (p.latitude == 0.0)
            cout << "GPS not localized\n";
        /*char c;

        iss >> c;
        iss >> l;// time

        iss >> c;//,
        iss >> p.latitude;

        iss >> c;//,
        iss >> c;//N
        iss >> c;//,
        iss >> p.longitude;
*/
        // printf("gps> lon: %f lat:%f", p.longitude, p.latitude);

        Ll tmp = nmea_to_decimal(p);

        // cout << "LAT: " << p.latitude << " LONG: " << p.longitude << " DEC: "
        // << tmp.latitude << ", " << tmp.longitude << '\n';
        pthread_mutex_lock(&m_read);
        data = tmp;
        pthread_mutex_unlock(&m_read);
    }
}

Ll GpsThread::nmea_to_decimal(Ll p) {
    Ll result;

    double deg = floor(p.latitude / 100);
    double dec_pos = deg + ((p.latitude - (deg * 100)) / 60);

    result.latitude = dec_pos;

    deg = floor(p.longitude / 100);
    dec_pos = deg + ((p.longitude - (deg * 100)) / 60);

    result.longitude = dec_pos;

    return result;
}