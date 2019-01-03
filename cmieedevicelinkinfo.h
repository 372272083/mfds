#ifndef CMIEEDEVICELINKINFO_H
#define CMIEEDEVICELINKINFO_H

#include "devicelinkinfo.h"

class CMIEEDeviceLinkInfo : public DeviceLinkInfo
{
    Q_OBJECT
public:
    CMIEEDeviceLinkInfo(QObject *parent = 0);

    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QByteArray getSendMsg();
    virtual QString getComAddress();

    virtual QString getParam(int type);

    void init();
private:
    bool syncTomerOk;
    bool comReadOk;
    bool factorReadOk;

    class TransformFactor
    {
    public:
        float zero_va;
        float zero_vb;
        float zero_vc;

        float zero_ca;
        float zero_cb;
        float zero_cc;

        float ratio_va;
        float ratio_vb;
        float ratio_vc;

        float ratio_ca;
        float ratio_cb;
        float ratio_cc;
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

        unsigned short data_format;

        unsigned short uar;
        unsigned short ubr;
        unsigned short ucr;

        unsigned short iar;
        unsigned short ibr;
        unsigned short icr;

        unsigned short wave_num;
        unsigned short wave_sample;
        unsigned short update_rate;
        unsigned short wire_choose;
        unsigned short calculate_method;
    };
    ComSettingParams comSettingParams;
};

#endif // CMIEEDEVICELINKINFO_H
