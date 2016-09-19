
#ifndef HOKUYOTHREAD_H
#define HOKUYOTHREAD_H

#include "DataTypes.h"
#include "LocalizationAndPlaning.h"
#include <string>

#define RANGE_DATA_COUNT 1081
#define HOKUYO_PORT 10940
#define HOKUYO_ADDR "192.168.0.10"

using namespace std;

class HokuyoThread {

  public:
    HokuyoThread(int is_online_mode);

    void init();
    void run();
    void stop();

    void getData(int* buffer);
    IplImage* getGuiFrame(int* dataArr);
    static int demands_immediate_stop() { int hdis = hokuyo_demands_immediate_stop; hokuyo_demands_immediate_stop = 0; return hdis; }

  private:
    static bool end;
    static pthread_mutex_t m_read;
    static int* data;
    static int sockfd;
    static int guiWidth;
    static int guiHeight;
    static IplImage* result;
    static int hokuyo_demands_immediate_stop;

    pthread_t t;

    static void* mainLoop(void*);
};

#endif /* HOKUYOTHREAD_H */
