#include <string.h>

#include "SbotThread.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <linux/unistd.h>

//#include <cstdlib>
#include <iostream>
#include <sstream>
//#include <iosfwd>
#include <ios>
#include <string>
//#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

using namespace std;

bool SbotThread::end;
// string SbotThread::data;
SbotData SbotThread::data;
// string SbotThread::buffer;
pthread_mutex_t SbotThread::m_read;
pthread_mutex_t SbotThread::m_write;
FILE* SbotThread::robot;

int SbotThread::fdR[2];
int SbotThread::fdW[2];
int SbotThread::child;

char SbotThread::device_name[32];

int SbotThread::printingDebugging;

static int online_mode;


SbotThread::SbotThread(int is_online_mode) {
    //    end = false;
    online_mode = is_online_mode;
    printingDebugging = 1;
    outlog = fopen("../logs/sbot-out.log", "a");
    end = true; // TODO:
}

void SbotThread::setDeviceName(const char* dev_name) {
    strcpy(device_name, dev_name);
    end = false;
}

int SbotThread::validate(const char* devName) {
    // char *command="plink /dev/ttyUSB0 -serial -sercfg 115200,N,n,8,1";
    char command[128];

    if (!online_mode) return 1;
    
    sprintf(command, "plink %s -serial -sercfg 115200,N,n,8,1", devName);

    FILE* f;

    if (!(f = (FILE*)popen(command, "r"))) {
        // If fpipe is NULL
        return 0;
    }
    if (f < 0)
        return 0;

    char b[128];

    for (int i = 0; i < 10; i++) {

        fgets(b, sizeof b, f);

        //@0 -6 0 0 0 0 200 108 200
        istringstream is(b);
        if (!is.eof()) {
            char c;
            int n;

            is >> c;
            if (is.fail() || c != '@') {
                continue;
            }

            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }
            is >> n;
            if (is.fail()) {
                continue;
            }

            // printf("%s", b );
            pclose(f);
            return 1;
        }
    }

    pclose(f);

    return 0;
}

int away_from_left;
int away_from_right;

void* SbotThread::mainLoop(void*) {

    // char *command="plink /dev/ttyUSB0 -serial -sercfg 115200,N,n,8,1";

    if (online_mode)
    {
        if (pipe(fdR) < 0) {
            perror("pipe2()");
            exit(-1);
        }
        if (!pipe(fdW) < 0) {
            perror("pipe2()");
            exit(-1);
        }

        if ((child = fork()) == 0) {
            /* child */

            close(0);
            close(1);
            dup2(fdR[0], 0);
            dup2(fdW[1], 1);
            close(fdR[0]);
            close(fdR[1]);
            close(fdW[0]);
            close(fdW[1]);

            // if (execl("/usr/bin/plink", "/usr/bin/plink", "/dev/ttyUSB0",
            // "-serial", "-sercfg", "115200,N,n,8,1", NULL) < 0) {
            if (execl("/usr/bin/plink", "/usr/bin/plink", device_name, "-serial",
                      "-sercfg", "115200,N,n,8,1", NULL) < 0) {
                perror("child execl()");
                exit(-1);
            }
        }
    }
    else child = 123;  // "a fake pid"
    
    if (child < 0) {
            perror("fork()");
            exit(-1);
    }

    /* parent */
    if (online_mode)
    {
        close(fdR[0]);
        close(fdW[1]);
    }
    char line[1024];

    FILE* inlog;
    inlog = fopen("../logs/sbot-in.log", "a");

    while (!end) {
        // wait for '@'
        unsigned char ch;
        int numRead;
        int printing = 0;
        int lineIndex = 1;

        if (online_mode)
        {
            do {
                if ((numRead = read(fdW[0], &ch, 1)) < 0) {
                    perror("read()");
                    exit(-1);
                }
                if (ch == '!')
                    printing = printingDebugging;
                if ((printing) && (ch == 13))
                    printing = 0;
                if (printing)
                    printf("%c", ch);
            } while (ch != '@');
            line[0] = '@';
        
            do {
                if ((numRead = read(fdW[0], line + lineIndex, 1)) < 0) {
                    perror("read()");
                    exit(-1);
                }
                lineIndex += numRead;
                if (lineIndex > 1023)
                    break;
            } while (line[lineIndex - 1] != '\n');
        }
        else
        {
            // TODO: read from log file
            strcpy(line, "@125 95 0 0 0 0 221 344 331 347 270\n");
            lineIndex = strlen(line) - 1;
            usleep(100000);
        }
        
        if (lineIndex > 1023)
            continue;
        line[lineIndex] = '\0';

        // fprintf(inlog, "%ld %s", time(NULL) ,line);
        time_t t = time(NULL);
        fprintf(inlog, "%s|%s", ctime(&t), line);
        fflush(inlog);

        // printf("%s",line);

        SbotData* result = new SbotData;

        int blocked, obstacle;

        sscanf(line, "@ %d %d %d %d %d %d %d %d %d %d %d", &result->lstep,
               &result->rstep, &result->lspeed, &result->rspeed, &blocked,
               &obstacle, &result->distRR, &result->distFR, &result->distM,
               &result->distFL, &result->distRL);

        result->blocked = (blocked);
        result->obstacle = (obstacle);

        if ((result->distRL < 35) && (result->distRL > 14))
            away_from_left = 1;
        else
            away_from_left = 0;
        if ((result->distRR < 35) && (result->distRR > 14))
            away_from_right = 1;
        else
            away_from_right = 0;

        pthread_mutex_lock(&m_read);
        SbotThread::data = *result;
        pthread_mutex_unlock(&m_read);
        /*
                    if (result->obstacle)
                    {
                        for (int ii = 0; ii < 30; i++)
                        {
                            sleep(1);

                        }
                    } */
        delete result;
        usleep(10000);
    }    
}

void SbotThread::run() {

    if (end) {
        return;
    }

    pthread_mutex_init(&m_read, NULL);
    pthread_mutex_init(&m_write, NULL);

    pthread_create(&t, NULL, mainLoop, NULL);
}

void SbotThread::stop() {
    if (!end) {
        end = true;
        if (pthread_join(t, NULL) != 0) {
            printf("sbot thread join error");
        }

        fclose(outlog);
    }
}

void SbotThread::sendCommand(const char* s) {

    if (end) {
        return;
    }
    if (!online_mode) return;

    pthread_mutex_lock(&m_write);

    // printf("command %s\n", s);
    // fprintf(outlog, "%ld %s\n", time(NULL),s);
    time_t t = time(NULL);
    fprintf(outlog, "%s|%s", ctime(&t), s);
    fflush(outlog);

    write(fdR[1], s, strlen(s));

    //    fputs( s, robot );
    //    fflush( robot);

    pthread_mutex_unlock(&m_write);
}

void SbotThread::setDirection(int d) {
    char buff[32];

    sprintf(buff, "d %d;", d);
    sendCommand(buff);
}

void SbotThread::setMaxSpeed(int maxSpeed) {
    char buff[32];

    sprintf(buff, "m %d;", maxSpeed);
    sendCommand(buff);
}

void SbotThread::stopNow() {
    char buff[32];
    sprintf(buff, "/");
    sendCommand(buff);
}

void SbotThread::setSpeed(int s) {
    char buff[32];

    sprintf(buff, "s %d;", s * 2);
    sendCommand(buff);
}

void SbotThread::unblock() { sendCommand("u;"); }

void SbotThread::ignoreObstacle(bool val) {
    if (val) {
        sendCommand("i;");
    } else {
        sendCommand("o;");
    }
}

SbotData SbotThread::getData() {
    SbotData result;

    if (end) {
        return result;
    }

    pthread_mutex_lock(&m_read);

    result = data;

    pthread_mutex_unlock(&m_read);

    return result;
}