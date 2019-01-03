#ifndef CMIEVDEVICELINKINFO_H
#define CMIEVDEVICELINKINFO_H

#include "devicelinkinfo.h"

class CMIEVDeviceLinkInfo : public DeviceLinkInfo
{
    Q_OBJECT
public:
    CMIEVDeviceLinkInfo(QObject *parent = 0);

    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QByteArray getSendMsg();
    virtual QString getComAddress();

    void init();
private:
    bool syncTomerOk;
    bool comReadOk;
    bool factorReadOk;

    class TransformFactor
    {
    public:
        float zero_x1;
        float zero_y1;
        float zero_z1;

        float zero_x2;
        float zero_y2;
        float zero_z2;

        float ratio_x1;
        float ratio_y1;
        float ratio_z1;

        float ratio_x2;
        float ratio_y2;
        float ratio_z2;
    };
    TransformFactor transFactor;

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

#endif // CMIEVDEVICELINKINFO_H
