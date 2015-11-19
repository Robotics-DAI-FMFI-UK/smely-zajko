#include "BindSerialPorts.h"

BindSerialPorts::BindSerialPorts() {
}

BindSerialPorts::~BindSerialPorts() {
}

void BindSerialPorts::bind( SbotThread *sbot, ImuThread *imu ){

    bool portsUsed[5];
    for(int i=0; i<5; i++ ){
        portsUsed[i] = false;
    }

    char dev_name[16];
    bool res = 0;

    
    if(sbot!=NULL){
        res = 0;

        sprintf(dev_name, "/dev/sbot");

                res = sbot->validate(dev_name);

                if( res ){
                    printf("sbot found: %s\n", dev_name);
                    sbot->setDeviceName( dev_name );

                }


        if(!res){
            printf("sbot not found.\n");
        }
        
    }

    if(imu!=NULL){
        res = 0;

        sprintf(dev_name, "/dev/imu");

                res = imu->validate(dev_name);

                if( res ){
                    printf("imu found: %s\n", dev_name);
                    imu->setDeviceName( dev_name );

                }

        if(!res){
            printf("imu not found.\n");
        }

    }

}