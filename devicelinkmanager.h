#ifndef DEVICELINKMANAGER_H
#define DEVICELINKMANAGER_H

#include <QObject>

#include "devicelinkinfo.h"
#include <QMap>
#include "xsocketclient.h"

class QTcpSocket;
class SqliteDB;

class DeviceLinkManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceLinkManager(SqliteDB *db, QObject *parent = 0);
    ~DeviceLinkManager();

    void connectDevice();
    QMap<QString,DeviceLinkInfo*> getDevicesInfo();

private:
    SqliteDB *m_db;
    int baseInterval;
    QMap<QString,XSocketClient*> m_sockes;
    QMap<QString,DeviceLinkInfo*> m_deviceInfo;

signals:

public slots:
    void signalOnReceiveData(QByteArray by,QString guid);
    void signalOnConnected(QString guid);
    void signalOnDisconnected(QString guid);

    void timeUpdate();
};

#endif // DEVICELINKMANAGER_H
