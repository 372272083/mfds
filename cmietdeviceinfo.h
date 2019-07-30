#ifndef CMIETDEVICEINFO_H
#define CMIETDEVICEINFO_H

#include "deviceinfo.h"

class CMIETDeviceInfo : public DeviceInfo
{
    Q_OBJECT
public:
    CMIETDeviceInfo(DeviceInfo *parent = 0);
    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QString getComAddress();

    void init();
    virtual void close();

private:
    bool syncTomerOk;
    bool comReadOk;

    class ComSettingParams
    {
    public:
        unsigned short ip_h;
        unsigned short ip_l;
        unsigned short ip_mask_h;
        unsigned short ip_mask_l;
        unsigned short ip_gate_h;
        unsigned short ip_gate_l;

        unsigned short rs485_baud_1;
        unsigned short rs485_addr_1;

        unsigned short rs485_baud_2;
        unsigned short rs485_addr_2;
    };
    ComSettingParams comSettingParams;
};

#endif // CMIETDEVICEINFO_H
