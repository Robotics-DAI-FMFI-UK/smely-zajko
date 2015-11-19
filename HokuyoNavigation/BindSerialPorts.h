#ifndef BINDSERIALPORTS_H
#define	BINDSERIALPORTS_H

#include "ImuThread.h"
#include "SbotThread.h"


class BindSerialPorts {
public:
    BindSerialPorts();
    virtual ~BindSerialPorts();

    void bind( SbotThread* sbot, ImuThread* imu );
private:

};

#endif	/* BINDSERIALPORTS_H */

