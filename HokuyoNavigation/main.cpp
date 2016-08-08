#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "SbotThread.h"
#include "BindSerialPorts.h"
#include "ImuThread.h"
#include "HokuyoThread.h"

bool START_TIMER = 0;


FILE* mainLog;
int log_counter=0;
bool autonomy = true;

ImuThread* imu;
SbotThread* sbot;
HokuyoThread *hokuyo;

char running = 1;
bool obchadzam = false;


void cuvat()
{
    printf("cuvam\n");
    sbot->setSpeed(-2);
    sbot->setDirection(0);

}

void okolo()
{
    printf("obchadzam prekazku\n");
    sbot->setDirection(60);
    sbot->setSpeed(1);
    obchadzam = true;
}

void vyvaz()
{
    printf("vraciam sa\n");
    sbot->setDirection(-60);
    sbot->setSpeed(1);
    obchadzam = false;
}

void chod()
{
    printf("idem\n");
    sbot->setSpeed(4);
    sbot->setDirection(0);
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "C");
    time_t t;
    time(&t);
        
    imu = new ImuThread();
    sbot = new SbotThread();
    hokuyo = new HokuyoThread();

    BindSerialPorts bs;
    bs.bind( sbot, imu );
    
    imu->init();
    hokuyo->init();
     
    sbot->run();    
    imu->run();
    hokuyo->run();
    
    sbot->ignoreObstacle(true);
    
    int laser[RANGE_DATA_COUNT];
    
    while (running)
    {
        hokuyo->getData(laser);
        int cuvaj = 0;
        for (int i = 0; i < 20; i++)
        {
            int sum = 0;      
            for (int j = 0; j < 1080/20; j++)
                sum += laser[i * (1080/20) + j];
            printf("%d ", sum / (1080/20));
            if (i == 10 || i == 9 || i == 11)
            {
                printf("<- ");            
                if ((sum / (1080/20)) < 1000)
                    cuvaj = 1;
                else if ((sum / (1080/20)) < 1500)
                    cuvaj = 2;
                else if ((sum / (1080/20)) > 3000 && obchadzam)
                    cuvaj = 3;
            }
        }
        printf("\n");
        
        if (cuvaj == 1){
            cuvat();
        }
        else if(cuvaj == 2){
            okolo();
            usleep(300000);
        }
        else if(cuvaj == 3){
            vyvaz();
            usleep(300000);
        }
        else {
            chod();
        }
                
        usleep(300000);
    }
    
    imu->stop();
    hokuyo->stop();
    sbot->stop();
    
    return 0;
}
