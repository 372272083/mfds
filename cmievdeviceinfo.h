#ifndef CMIEVDEVICEINFO_H
#define CMIEVDEVICEINFO_H

#include "deviceinfo.h"
#include <QVector>

class CMIEVAnalyseThread;

class CMIEVDeviceInfo : public DeviceInfo
{
    Q_OBJECT
public:
    CMIEVDeviceInfo(DeviceInfo *parent = 0);

    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QString getComAddress();
    virtual QString getParam(int type);

    void init();
    virtual void close();
private:
    QVector<CMIEVAnalyseThread*> analyse_threads;
    int seqence_index;
    bool syncTomerOk;
    bool comReadOk;
    bool factorReadOk;
    bool modelWriteOk;
    bool modelReadOk;

    int run_mode;

    class Ratio
    {
    public:
        float ratio_1;
        float ratio_2;
        float ratio_3;
        float ratio_4;
        float ratio_5;
        float ratio_6;
    };
    Ratio ratio;

    float rotate;

    class TransformFactor
    {
    public:
        unsigned short zero_x1;
        unsigned short zero_y1;
        unsigned short zero_z1;

        unsigned short zero_x2;
        unsigned short zero_y2;
        unsigned short zero_z2;

        unsigned short ratio_x1;
        unsigned short ratio_y1;
        unsigned short ratio_z1;

        unsigned short ratio_x2;
        unsigned short ratio_y2;
        unsigned short ratio_z2;
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

#endif // CMIEVDEVICEINFO_H
