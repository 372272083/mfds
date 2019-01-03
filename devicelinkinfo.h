#ifndef DEVICELINKINFO_H
#define DEVICELINKINFO_H

#include <QObject>
#include "constants.h"
#include "devicepipeinfo.h"

#include <QQueue>


class DeviceLinkInfo : public QObject
{
    Q_OBJECT
public:
    explicit DeviceLinkInfo(QObject *parent = 0);
    virtual ~DeviceLinkInfo();

    QString deviceCode;
    QString deviceName;
    //QString attachMotor;
    QString deviceIpAddress;
    int ipPort;
    QString deviceType;
    QString deviceModel;

    bool linkState;

    int waitingCount;
    int reConnectCount;

    virtual void handlerReceiveMsg();
    virtual void handleSendMsg();
    virtual QByteArray getSendMsg();
    virtual QString getComAddress();
    virtual QString getParam(int type);

    void addPipe(QString name,QString mcode,QString dcode,QString dname,QString dtype,QString pipecode);

    QList<DevicePipeInfo*> pipes;

    void addrevdata(QByteArray recv);
    bool isExpLen();

    void init();

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

signals:

public slots:
};

#endif // DEVICELINKINFO_H
