#include "devicelinkmanager.h"

#include "cmieedeviceinfo.h"
#include "cmievdeviceinfo.h"
#include "cmietdeviceinfo.h"

//#include "sqlitedb.h"
#include "constants.h"

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QTimer>
#include <QDateTime>

#include <QDebug>

#include "globalvariable.h"

DeviceLinkManager::DeviceLinkManager(/*SqliteDB *db, */QObject *parent) : QObject(parent),/*m_db(db),*/baseInterval(BASE_INTERVAL)
{
    state_s = true;
}

void DeviceLinkManager::timeUpdate()
{
    static int count = 0;
    static int countDisconnect = 0;

    countDisconnect++;
    if(!GlobalVariable::isOnline || !state_s)
    {
        return;
    }
    //QMap<QString,DeviceInfo*>::const_iterator it;
    //for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it ) {
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=GlobalVariable::deviceInfos.constBegin();cIt != GlobalVariable::deviceInfos.constEnd();cIt++)
    {
        //QString devicecode = it.key();
        DeviceInfo* deviceInfo = *cIt;
        QString devicecode = deviceInfo->deviceCode;
        //qDebug() << "cycle msg: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat) << " : " << devicecode;
        if (deviceInfo->linkState)
        {
            //qDebug() << "cycle msg: " << "1";
            if(countDisconnect > 7200)
            {
                countDisconnect = 0;
                if(m_sockes.contains(devicecode))
                {
                    XSocketClient* client = m_sockes[devicecode];
                    client->disConnect();
                    continue;
                }
            }
            deviceInfo->handleSendMsg();
            QByteArray msg = deviceInfo->getSendMsg();

            if (msg.size() > 0)
            {
                qDebug() << "cycle msg: " << "11";
                if(m_sockes.contains(devicecode))
                {
                    XSocketClient* client = m_sockes[devicecode];

                    if (nullptr != client)
                    {
                        client->SendData(msg);
                    }
                }
            }
            else
            {
                qDebug() << "cycle msg: " << "12";
                deviceInfo->waitingCount++;
                if((deviceInfo->waitingCount/2) > GlobalVariable::sample_waiting)
                {
                    XSocketClient* client = m_sockes[devicecode];
                    if(client != nullptr)
                    {
                        client->disConnect();
                        deviceInfo->waitingCount = 0;
                    }
                }
            }
        }
        else
        {
            //qDebug() << "cycle msg: " << "2";
            if (0 == deviceInfo->reConnectCount)
            {
                XSocketClient* client = m_sockes[devicecode];
                if(client != nullptr)
                {
                    client->disConnect();
                    m_sockes.remove(devicecode);
                    delete client;
                }
            }
            deviceInfo->reConnectCount++;
            if ((deviceInfo->reConnectCount/2) > GlobalVariable::sample_waiting)
            {
                QString ipAddress = deviceInfo->ipAddress;
                deviceInfo->init();
                int port = deviceInfo->ipPort;
                //qDebug() << devicecode << it.value()->deviceIpAddress;
                XSocketClient *client = new XSocketClient(devicecode);
                //connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
                connect(client,SIGNAL(signalOnReceiveData(QByteArray,QString)),this,SLOT(signalOnReceiveData(QByteArray,QString)));
                connect(client,SIGNAL(signalOnConnected(QString)),this,SLOT(signalOnConnected(QString)));
                connect(client,SIGNAL(signalOnDisconnected(QString)),this,SLOT(signalOnDisconnected(QString)));

                client->ConnectTo(ipAddress,port);
                if(m_sockes.contains(devicecode))
                {
                    XSocketClient* client_tmp = m_sockes[devicecode];
                    if(client_tmp != nullptr)
                    {
                        client_tmp->disConnect();
                        m_sockes.remove(devicecode);
                        delete client_tmp;
                    }
                }
                m_sockes.insert(devicecode,client);
                deviceInfo->reConnectCount = 1;
            }
        }
    }

    count++;
}

void DeviceLinkManager::close()
{
    state_s = false;

    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=GlobalVariable::deviceInfos.constBegin();cIt != GlobalVariable::deviceInfos.constEnd();cIt++)
    {
        //QString devicecode = it.key();
        DeviceInfo* deviceInfo = *cIt;
        QString devicecode = deviceInfo->deviceCode;
        if (deviceInfo->linkState)
        {
            deviceInfo->close();
            if(deviceInfo->deviceType.compare("V",Qt::CaseInsensitive) == 0)
            {
                struct ModbusTCPMapInfo cmd_mode;
                cmd_mode.Unit = MODE_V_W;
                cmd_mode.Addr = 36;
                cmd_mode.Command = 0x10;
                cmd_mode.Length = 2;
                cmd_mode.data = new unsigned char[2];
                cmd_mode.data[0] = 0x0;
                cmd_mode.data[1] = 0x0;
                cmd_mode.ExpectLen = 15;

                deviceInfo->addSendMsg(cmd_mode);
                QByteArray msg = deviceInfo->getSendMsg();

                if (msg.size() > 0)
                {
                    if(m_sockes.contains(devicecode))
                    {
                        XSocketClient* client = m_sockes[devicecode];

                        if (nullptr != client)
                        {
                            client->SendData(msg);
                            int tmp = 0;
                            while(tmp>100)
                            {
                                tmp++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void DeviceLinkManager::disConnectDevice()
{
    QMap<QString,XSocketClient*>::const_iterator its;
    for ( its = m_sockes.constBegin(); its != m_sockes.constEnd(); ++its ) {
        XSocketClient* xsc = its.value();
        xsc->disConnect();
        delete xsc;
    }
    m_sockes.clear();
}

void DeviceLinkManager::connectDevice()
{
    //QMap<QString,DeviceInfo*>::const_iterator it;
    //for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it )
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=GlobalVariable::deviceInfos.constBegin();cIt != GlobalVariable::deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* info = *cIt;
        QString devicecode = info->deviceCode;
        QString ipAddress = info->ipAddress;
        int port = info->ipPort;
        //qDebug() << devicecode << it.value()->deviceIpAddress;
        XSocketClient *client = new XSocketClient(devicecode);
        //connect(pcancelbtn, SIGNAL(clicked()),this, SLOT(cancel()));
        connect(client,SIGNAL(signalOnReceiveData(QByteArray,QString)),this,SLOT(signalOnReceiveData(QByteArray,QString)));
        connect(client,SIGNAL(signalOnConnected(QString)),this,SLOT(signalOnConnected(QString)));
        connect(client,SIGNAL(signalOnDisconnected(QString)),this,SLOT(signalOnDisconnected(QString)));

        client->ConnectTo(ipAddress,port);
        m_sockes.insert(devicecode,client);
    }

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));
    timer->start(baseInterval/2);
}

QMap<QString,DeviceInfo*> DeviceLinkManager::getDevicesInfo()
{
    return m_deviceInfo;
}

void DeviceLinkManager::signalOnReceiveData(QByteArray by,QString guid)
{
    //qDebug()<< "received data: " << guid;
    DeviceInfo* deviceInfo = GlobalVariable::findDeviceByCode(guid);// m_deviceInfo[guid];
    if (nullptr != deviceInfo)
    {
        deviceInfo->addrevdata(by);
        if (deviceInfo->isExpLen())
        {
            deviceInfo->handlerReceiveMsg();
        }
    }
}

void DeviceLinkManager::signalOnConnected(QString guid)
{
    qDebug()<< "onConnected: " <<guid;
    DeviceInfo* deviceInfo = GlobalVariable::findDeviceByCode(guid);

    if(deviceInfo != nullptr)
    {
        deviceInfo->init();
        deviceInfo->linkState = true;
        deviceInfo->reConnectCount = 0;
        emit linkStateChanged(deviceInfo->tree_id,true);
    }
}

void DeviceLinkManager::signalOnDisconnected(QString guid)
{
    qDebug()<< "onDisconnected: " <<guid;
    DeviceInfo* deviceInfo = GlobalVariable::findDeviceByCode(guid);

    if(deviceInfo != nullptr)
    {
        deviceInfo->linkState = false;
        deviceInfo->reConnectCount = 0;
        emit linkStateChanged(deviceInfo->tree_id,false);
    }
}

DeviceLinkManager::~DeviceLinkManager()
{
    QMap<QString,XSocketClient*>::const_iterator its;
    for ( its = m_sockes.constBegin(); its != m_sockes.constEnd(); ++its ) {
        XSocketClient* xsc = its.value();
        xsc->disConnect();
        delete xsc;
    }
    m_sockes.clear();

    /*
    QMap<QString,DeviceInfo*>::const_iterator it;
    for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it ) {
        DeviceInfo* dli = it.value();
        delete dli;
    }
    m_deviceInfo.clear();
    */
}
