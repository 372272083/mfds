#ifndef CMIETDEVICELINKINFO_H
#define CMIETDEVICELINKINFO_H

#include "devicelinkinfo.h"

class CMIETDeviceLinkInfo : public DeviceLinkInfo
{
    Q_OBJECT
public:
    CMIETDeviceLinkInfo(QObject *parent = 0);
    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QByteArray getSendMsg();
    virtual QString getComAddress();

    void init();

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

#endif // CMIETDEVICELINKINFO_H
