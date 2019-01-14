#include "devicelinkmanager.h"

#include "cmieedevicelinkinfo.h"
#include "cmievdevicelinkinfo.h"
#include "cmietdevicelinkinfo.h"

#include "sqlitedb.h"
#include "constants.h"

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QTimer>
#include <QDateTime>

#include <QDebug>

#include "mainwindow.h"

DeviceLinkManager::DeviceLinkManager(SqliteDB *db, QObject *parent) : QObject(parent),m_db(db),baseInterval(BASE_INTERVAL)
{
    QSqlTableModel *model = m_db->modelNoHeader("device");
    QSqlTableModel *pipemodel = m_db->modelNoHeader("devicepipes");

    int rows = model->rowCount();
    QString deviceModel_l;
    QString deviceCode_l;
    QString deviceName_l;
    QString attachMotor_l;
    QString deviceIpAddress_l;
    int ipPort_l;
    QString deviceType_l;
    int isactive_l;
    int pipeisactive_l;
    for(int i=0;i<rows;i++)
    {
        QSqlRecord record = model->record(i);
        deviceModel_l = record.value("dmodel").toString();
        deviceCode_l = record.value("code").toString();
        deviceName_l = record.value("name").toString();
        //attachMotor_l = record.value("motor").toString();
        deviceType_l = record.value("dtype").toString();
        deviceIpAddress_l = record.value("ipaddress").toString();
        ipPort_l = record.value("port").toInt();
        isactive_l = record.value("isactive").toInt();

        pipemodel->setFilter("dcode='"+deviceCode_l+"'");
        pipemodel->select();

        int tempRows = pipemodel->rowCount();
        if (0 == isactive_l)
        {
            continue;
        }
        if (deviceModel_l.compare("CMIE-E") == 0) //e7 electirc sample device
        {
            CMIEEDeviceLinkInfo *eInfo = new CMIEEDeviceLinkInfo();
            eInfo->init();
            eInfo->deviceCode = deviceCode_l;
            eInfo->deviceName = deviceName_l;
            //eInfo->attachMotor = attachMotor_l;
            eInfo->deviceIpAddress = deviceIpAddress_l;
            eInfo->ipPort = ipPort_l;
            eInfo->deviceType = deviceType_l;
            eInfo->deviceModel = deviceModel_l;

            for(int n=0;n<tempRows;n++)
            {
                QSqlRecord recordpipe = pipemodel->record(n);
                QString pipedcode = recordpipe.value("dcode").toString();
                QString pipemcode = recordpipe.value("motor").toString();
                QString pipename = recordpipe.value("name").toString();
                QString pipecode = recordpipe.value("pipeCode").toString();
                pipeisactive_l = recordpipe.value("isactive").toInt();

                if (1 == pipeisactive_l)
                {
                    eInfo->addPipe(pipename,pipemcode,pipedcode,eInfo->deviceName,eInfo->deviceType,pipecode);
                }
            }

            m_deviceInfo.insert(deviceCode_l,eInfo);
        }
        else if (deviceModel_l.compare("CMIE-V") == 0) //e7 vibrate sample device
        {
            CMIEVDeviceLinkInfo *vInfo = new CMIEVDeviceLinkInfo();
            vInfo->init();
            vInfo->deviceCode = deviceCode_l;
            vInfo->deviceName = deviceName_l;
            //vInfo->attachMotor = attachMotor_l;
            vInfo->deviceIpAddress = deviceIpAddress_l;
            vInfo->ipPort = ipPort_l;
            vInfo->deviceType = deviceType_l;
            vInfo->deviceModel = deviceModel_l;

            for(int n=0;n<tempRows;n++)
            {
                QSqlRecord recordpipe = pipemodel->record(n);
                QString pipedcode = recordpipe.value("dcode").toString();
                QString pipemcode = recordpipe.value("motor").toString();
                QString pipename = recordpipe.value("name").toString();
                QString pipecode = recordpipe.value("pipeCode").toString();

                pipeisactive_l = recordpipe.value("isactive").toInt();

                if (1 == pipeisactive_l)
                {
                    vInfo->addPipe(pipename,pipemcode, pipedcode,vInfo->deviceName,vInfo->deviceType,pipecode);
                }
            }

            m_deviceInfo.insert(deviceCode_l,vInfo);
        }
        else if (deviceModel_l.compare("CMIE-T") == 0) //e7 vibrate sample device
        {
            CMIETDeviceLinkInfo *tInfo = new CMIETDeviceLinkInfo();
            tInfo->init();
            tInfo->deviceCode = deviceCode_l;
            tInfo->deviceName = deviceName_l;
            //tInfo->attachMotor = attachMotor_l;
            tInfo->deviceIpAddress = deviceIpAddress_l;
            tInfo->ipPort = ipPort_l;
            tInfo->deviceType = deviceType_l;
            tInfo->deviceModel = deviceModel_l;

            for(int n=0;n<tempRows;n++)
            {
                QSqlRecord recordpipe = pipemodel->record(n);
                QString pipedcode = recordpipe.value("dcode").toString();
                QString pipemcode = recordpipe.value("motor").toString();
                QString pipename = recordpipe.value("name").toString();
                QString pipecode = recordpipe.value("pipeCode").toString();

                pipeisactive_l = recordpipe.value("isactive").toInt();

                if (1 == pipeisactive_l)
                {
                    tInfo->addPipe(pipename,pipemcode,pipedcode,tInfo->deviceName,tInfo->deviceType,pipecode);
                }
            }

            m_deviceInfo.insert(deviceCode_l,tInfo);
        }
        else
        {
            DeviceLinkInfo *info = new DeviceLinkInfo();
            info->init();
            info->deviceCode = deviceCode_l;
            info->deviceName = deviceName_l;
            //info->attachMotor = attachMotor_l;
            info->deviceIpAddress = deviceIpAddress_l;
            info->ipPort = ipPort_l;
            info->deviceType = deviceType_l;
            info->deviceModel = deviceModel_l;

            for(int n=0;n<tempRows;n++)
            {
                QSqlRecord recordpipe = pipemodel->record(n);
                QString pipedcode = recordpipe.value("dcode").toString();
                QString pipemcode = recordpipe.value("motor").toString();
                QString pipename = recordpipe.value("name").toString();
                QString pipecode = recordpipe.value("pipeCode").toString();

                pipeisactive_l = recordpipe.value("isactive").toInt();

                if (1 == pipeisactive_l)
                {
                    info->addPipe(pipename,pipemcode,pipedcode,info->deviceName,info->deviceType,pipecode);
                }
            }

            m_deviceInfo.insert(deviceCode_l,info);
        }
    }

    delete model;
    delete pipemodel;
}

void DeviceLinkManager::timeUpdate()
{
    static int count = 0;
    static int countDisconnect = 0;

    countDisconnect++;
    if(!MainWindow::com_enable)
    {
        return;
    }
    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it ) {
        QString devicecode = it.key();
        DeviceLinkInfo* deviceInfo = it.value();
        if (deviceInfo->linkState)
        {
            if(countDisconnect > 3600)
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
                deviceInfo->waitingCount++;
                if(deviceInfo->waitingCount > MainWindow::sample_waiting)
                {
                    XSocketClient* client = m_sockes[devicecode];
                    client->disConnect();
                    deviceInfo->waitingCount = 0;
                }
            }
        }
        else
        {
            if (0 == deviceInfo->reConnectCount)
            {
                XSocketClient* client = m_sockes[devicecode];
                client->disConnect();
                m_sockes.remove(devicecode);
                delete client;
            }
            deviceInfo->reConnectCount++;
            if (deviceInfo->reConnectCount > MainWindow::sample_waiting)
            {
                QString ipAddress = deviceInfo->deviceIpAddress;
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
                    client_tmp->disConnect();
                    m_sockes.remove(devicecode);
                    delete client_tmp;
                }
                m_sockes.insert(devicecode,client);
                deviceInfo->reConnectCount = 1;
            }
        }
    }

    if (0 == MainWindow::data_save_days)
    {
        if (count % 3600 == 0)
        {
            QQueue<QString> sqls;
            QString sql = "delete from electriccharge";
            sqls.enqueue(sql);

            sql = "delete from electricchargewave";
            sqls.enqueue(sql);

            sql = "delete from electricchargewavefreq";
            sqls.enqueue(sql);

            sql = "delete from vibrate";
            sqls.enqueue(sql);

            sql = "delete from vibratewave";
            sqls.enqueue(sql);

            sql = "delete from vibratewavefreq";
            sqls.enqueue(sql);

            sql = "delete from temperature";
            sqls.enqueue(sql);

            m_db->execSql(sqls);

            count = 0;
        }
    }
    else if((count % (3600*24*MainWindow::data_save_days)) == 0)
    {
        int tmpDay = 0 - MainWindow::data_save_days;
        QDateTime current_time = QDateTime::currentDateTime();
        current_time = current_time.addDays(tmpDay);
        //current_time = current_time.addSecs(-10);
        QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");

        QString sql = "delete from electriccharge where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from electricchargewave where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from electricchargewavefreq where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from vibrate where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from vibratewave where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from vibratewavefreq where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        sql = "delete from temperature where rksj <'" + StrCurrentTime + "'";
        m_db->updatasql(sql);

        count = 0;
    }
    count++;
}

void DeviceLinkManager::connectDevice()
{
    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it ) {
        QString devicecode = it.key();
        QString ipAddress = it.value()->deviceIpAddress;
        int port = it.value()->ipPort;
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
    timer->start(baseInterval);
}

QMap<QString,DeviceLinkInfo*> DeviceLinkManager::getDevicesInfo()
{
    return m_deviceInfo;
}

void DeviceLinkManager::signalOnReceiveData(QByteArray by,QString guid)
{
    qDebug()<< "received data: " << guid;
    DeviceLinkInfo* deviceInfo = m_deviceInfo[guid];
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
    m_deviceInfo[guid]->linkState = true;
    m_deviceInfo[guid]->reConnectCount = 0;
}

void DeviceLinkManager::signalOnDisconnected(QString guid)
{
    qDebug()<< "onDisconnected: " <<guid;
    m_deviceInfo[guid]->linkState = false;
    m_deviceInfo[guid]->reConnectCount = 0;
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

    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for ( it = m_deviceInfo.constBegin(); it != m_deviceInfo.constEnd(); ++it ) {
        DeviceLinkInfo* dli = it.value();
        delete dli;
    }
    m_deviceInfo.clear();
}
