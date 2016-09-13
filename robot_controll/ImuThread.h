#ifndef IMUTHREAD_H
#define IMUTHREAD_H

#include <errno.h>
#include <ios>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define COMPASS_ALIGNMENT 900

using namespace std;

class ImuData {
  public:
    double xAngle;
    double yAngle;
    double zAngle;
};

class ImuThread {

  public:
    ImuThread();

    void init();
    void run();
    void stop();

    ImuData getData();

    int validate(char* devName);
    void setDeviceName(char* dev_name);

  private:
    static char IMUPORT[32];

    static bool end;
    static pthread_mutex_t m_read;
    static int imu;
    static ImuData data;

    pthread_t t;

    static void* mainLoop(void*);
    static int readLine(char* s);
    static int imuOpen(char* filename);
};

#endif /* IMUTHREAD_H */
