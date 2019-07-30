#ifndef DEVICELINKMANAGER_H
#define DEVICELINKMANAGER_H

#include <QObject>

#include "deviceinfo.h"
#include <QMap>
#include "xsocketclient.h"

class QTcpSocket;
//class SqliteDB;

class DeviceLinkManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceLinkManager(/*SqliteDB *db, */QObject *parent = 0);
    ~DeviceLinkManager();

    void connectDevice();
    void disConnectDevice();
    void close();
    QMap<QString,DeviceInfo*> getDevicesInfo();

private:
    //SqliteDB *m_db;
    int baseInterval;
    bool state_s;
    QMap<QString,XSocketClient*> m_sockes;
    QMap<QString,DeviceInfo*> m_deviceInfo;

signals:
    void linkStateChanged(int treeId,bool state);
public slots:
    void signalOnReceiveData(QByteArray by,QString guid);
    void signalOnConnected(QString guid);
    void signalOnDisconnected(QString guid);

    void timeUpdate();
};

#endif // DEVICELINKMANAGER_H
