#ifndef BINDSERIALPORTS_H
#define	BINDSERIALPORTS_H

#include "GpsThread.h"
#include "ImuThread.h"
#include "SbotThread.h"


class BindSerialPorts {
public:
    BindSerialPorts();
    virtual ~BindSerialPorts();

    void bind( SbotThread* sbot, GpsThread* gps, ImuThread* imu );
private:

};

#endif	/* BINDSERIALPORTS_H */

