
#ifndef HOKUYOTHREAD_H
#define	HOKUYOTHREAD_H

#include "DataTypes.h"
#include "LocalizationAndPlaning.h"
#include <string>

#define RANGE_DATA_COUNT 1081
#define HOKUYO_PORT 10940
#define HOKUYO_ADDR "192.168.0.10"

using namespace std;

class HokuyoThread {

public:
    HokuyoThread();

    void init();
    void run();
    void stop();

    void getData(int *buffer);
    IplImage* getGuiFrame(int* dataArr);    

private:

    
    static bool end;
    static pthread_mutex_t m_read;
    static int *data;
    static int sockfd;
    static int guiWidth;
    static int guiHeight;
    static IplImage* result;
    
    pthread_t t;

    static void* mainLoop(void*);
    
};

#endif	/* HOKUYOTHREAD_H */
