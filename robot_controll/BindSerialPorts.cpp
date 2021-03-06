#include "BindSerialPorts.h"

BindSerialPorts::BindSerialPorts() {}

BindSerialPorts::~BindSerialPorts() {}

void BindSerialPorts::bind(SbotThread* sbot, GpsThread* gps, ImuThread* imu) {

    bool portsUsed[5];
    for (int i = 0; i < 5; i++) {
        portsUsed[i] = false;
    }

    char dev_name[16];
    bool res = 0;

    if (gps != NULL) {
        sprintf(dev_name, "/dev/galileo");
        /*
        for(int i=0; i<5; i++){
            sprintf(dev_name, "/dev/ttyUSB%d", i); */
        res = gps->validate(dev_name);

        if (res) {
            printf("gps found: %s\n", dev_name);
            gps->setDeviceName(dev_name);
            //                portsUsed[i] = true;
            //                break;
        }
        //}

        if (!res) {
            printf("gps not found.\n");
        }
    }

    if (sbot != NULL) {
        res = 0;

        sprintf(dev_name, "/dev/sbot");
        /* for(int i=0; i<5; i++){
            if( !portsUsed[i] ){
                sprintf(dev_name, "/dev/ttyUSB%d", i);  */
        res = sbot->validate(dev_name);

        if (res) {
            printf("sbot found: %s\n", dev_name);
            sbot->setDeviceName(dev_name);
            //                  portsUsed[i] = true;
            //                  break;
        }
        //  }
        //}

        if (!res) {
            printf("sbot not found.\n");
        }
    }

    if (imu != NULL) {
        res = 0;

        sprintf(dev_name, "/dev/imu");

        /* for(int i=0; i<5; i++){
            if( !portsUsed[i] ){
                sprintf(dev_name, "/dev/ttyUSB%d", i); */
        res = imu->validate(dev_name);

        if (res) {
            printf("imu found: %s\n", dev_name);
            imu->setDeviceName(dev_name);

            //                break;
        }
        //}
        //}

        if (!res) {
            printf("imu not found.\n");
        }
    }
}