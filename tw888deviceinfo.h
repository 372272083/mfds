#ifndef TW888DEVICEINFO_H
#define TW888DEVICEINFO_H

#include "deviceinfo.h"

class TW888DeviceINfo : public DeviceInfo
{
    Q_OBJECT
public:
    TW888DeviceINfo(DeviceInfo *parent = 0);
    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QString getComAddress();

    void init();
    virtual void close();

private:
    int area_id;
    int machine_id;
};

#endif // TW888DEVICEINFO_H