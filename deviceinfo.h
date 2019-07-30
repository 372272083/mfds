#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "constants.h"

#include "treenodeinfo.h"
#include "channelinfo.h"

#include <QQueue>

class DeviceInfo : public TreeNodeInfo
{
    Q_OBJECT
public:
    explicit DeviceInfo(TreeNodeInfo *parent = 0);
    virtual ~DeviceInfo();

    int tree_id;
    QString ipAddress;
    int ipPort;

    QString deviceCode;
    QString deviceType;
    QString deviceModel;

    QList<int> samples;
    QList<float> intervals;

    QList<float> filter_freqency_h_s;
    QList<float> filter_freqency_l_s;

    int measure_interval;
    int freq_interval;
    int wave_interval;

    bool linkState;

    int waitingCount;
    int reConnectCount;

    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QString getComAddress();
    virtual QString getParam(int type);
    virtual void init();
    virtual void close();

    void addPipe(QString name,QString mcode,QString dcode,QString dname,QString dtype,QString pipecode);

    QList<ChannelInfo*> pipes;

    void addrevdata(QByteArray recv);
    bool isExpLen();

    void addSendMsg(struct ModbusTCPMapInfo msg);
    QByteArray getSendMsg();

    static float bufferTofloat(QByteArray value,BYTEORDER4 bo=LL_LH_HL_HH);
    static ushort bufferToUShort(QByteArray value,BYTEORDER2 bo=LL_HH);

    static QByteArray floatToBuffer(float value,BYTEORDER4 bo=LL_LH_HL_HH);
    static QByteArray ushortToBuffer(ushort value,BYTEORDER2 bo=LL_HH);

protected:
    bool isReceiving;
    int expDataLen;
    QByteArray rec_buffer;
    QQueue<ModbusTCPMapInfo> msgSendQueue;
    QQueue<ModbusTCPMapInfo> msgPriSendQueue;

    QByteArray modbusEncomposeMsg(ModbusTCPMapInfo mtInfo);
    bool isHaveSomeState(int queuetype,MSGAddr type);

signals:

public slots:
};

#endif // DEVICEINFO_H
