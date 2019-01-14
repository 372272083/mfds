#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QSplitter>
#include <QTextEdit>
#include <QTabWidget>
#include <QTimer>
#include <QMessageBox>
#include <QList>
#include <QLabel>
#include <QPalette>

#include <QSqlTableModel>
#include <QSqlRecord>

#include <QCheckBox>

#include <QDir>
#include <QFont>
#include <QDebug>
#include <QScreen>

#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include <QGuiApplication>

#include "sqlitedb.h"
#include "motormanage.h"
#include "motortype.h"
#include "beartype.h"
#include "devicemanager.h"
#include "devicetype.h"
#include "devicepipe.h"
#include "devicelinkmanager.h"
#include "motorinfowidget.h"
#include "devicepipeselectwidget.h"
#include "chartinfocontroller.h"

#include "chargeinfo.h"
#include "vibrateinfo.h"
#include "temperatureinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"

#include "paramsetting.h"
#include "comsetting.h"

#include "constants.h"

#include "modbustcpserver.h"
#include "udisk.h"
#include "waveform.h"

bool MainWindow::com_enable = true;
bool MainWindow::show_enable = true;

bool MainWindow::virtual_data_enable = false;
bool MainWindow::desktop_capture_enable = false;

bool MainWindow::modbus_server_enable = false;
bool MainWindow::server_enable = false;

int MainWindow::freq_sample_interval = 10;
int MainWindow::wave_sample_interval = 1800;
int MainWindow::measure_sample_interval = 2;

int MainWindow::cmie_v_freq_len = 12296;
int MainWindow::cmie_v_wave_len = 196616;

int MainWindow::cmie_e_freq_len = 12296;
int MainWindow::cmie_e_wave_len = 196616;

int MainWindow::sample_waiting = 30;
QString MainWindow::server_ip = "127.0.0.1";
int MainWindow::server_waiting = 50;

QString MainWindow::group_name = "";
int MainWindow::data_save_days = 3;

QMap<QString,QQueue<ChargeInfo*>> MainWindow::charges;
QMap<QString,QQueue<VibrateInfo*>> MainWindow::vibrates;
QMap<QString,QQueue<TemperatureInfo*>> MainWindow::temperatures;

QMap<QString,QQueue<FreqInfo*>> MainWindow::freqs;
QMap<QString,QQueue<WaveInfo*>> MainWindow::waves;

QMap<QString,QQueue<FreqInfo*>> MainWindow::freqs_v;
QMap<QString,QQueue<WaveInfo*>> MainWindow::waves_v;

QQueue<QByteArray> MainWindow::trans_queue;
QQueue<QByteArray> MainWindow::trans_queue_pri;

QByteArray MainWindow::modbus_buffer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //QFont font;
    //font.setPixelSize(16);
    //menuBar()->setFixedHeight(30);
    //menuBar()->setFont(font);

    m_uDisk = nullptr;
    uDisk_enable = false;
    uDisk_flag = false;

    this->setWindowIcon(QIcon(":/images/icon"));

    QAction *userAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Login"),this);
    QAction *logoutAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Logout..."),this);
    QAction *exitAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Exit..."),this);
    connect(exitAction,&QAction::triggered,this,&MainWindow::close);
    QMenu *user = menuBar()->addMenu(tr("&User"));
    user->addAction(userAction);
    user->addAction(logoutAction);
    user->addAction(exitAction);

    QAction *motorAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Motor..."),this);
    connect(motorAction,&QAction::triggered,this,&MainWindow::motormanager);
    QAction *motortypeAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Motor Type"),this);
    connect(motortypeAction,&QAction::triggered,this,&MainWindow::motortypemanager);
    QAction *bearAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Bear Type"),this);
    connect(bearAction,&QAction::triggered,this,&MainWindow::bearmanager);
    QMenu *motor = menuBar()->addMenu(tr("&Motor"));
    motor->addAction(motorAction);
    motor->addAction(motortypeAction);
    motor->addAction(bearAction);

    QAction *devicetypeAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Device Model"),this);
    connect(devicetypeAction,&QAction::triggered,this,&MainWindow::devicetypemanager);
    QAction *deviceAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Device list"),this);
    connect(deviceAction,&QAction::triggered,this,&MainWindow::devicemanager);
    QAction *devicePipeAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Device Pipe"),this);
    connect(devicePipeAction,&QAction::triggered,this,&MainWindow::devicepipemanager);
    QMenu *device = menuBar()->addMenu(tr("&Device"));
    device->addAction(devicetypeAction);
    device->addAction(deviceAction);
    device->addAction(devicePipeAction);

    QAction *settingAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Setting..."),this);
    connect(settingAction,&QAction::triggered,this,&MainWindow::paramSetting);
    QAction *settingDeviceAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Device Setting"),this);
    connect(settingDeviceAction,&QAction::triggered,this,&MainWindow::deviceComSetting);
    QMenu *option = menuBar()->addMenu(tr("&Option"));
    option->addAction(settingAction);
    option->addAction(settingDeviceAction);

    QAction *aboutAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&About..."),this);
    QAction *versionAction = new QAction(/*QIcon(":/images/file-open"),*/tr("&Version..."),this);
    QMenu *about = menuBar()->addMenu(tr("&Help"));
    about->addAction(aboutAction);
    about->addAction(versionAction);

    QString cur_path = QCoreApplication::applicationDirPath();
    //QString dbfile = "D:/Program Files/mfds/data_mf.db";
    QString dbfile = cur_path + "/data_mf.db";
    qDebug() <<"current path: "<< dbfile << endl;
    db = new SqliteDB(&dbfile);
    if(db->open())
    {
        createtables();
    }
    else
    {
        QMessageBox::information(this, tr("Infomation"), tr("Read config file failed,application will exit!"));
        this->close();  //读取配置文件失败
    }

    QSplitter *pSpliterMain = new QSplitter(Qt::Horizontal);
    motorInfo = new MotorInfoWidget(db);
    motorInfo->setFixedWidth(WINDOW_LEFT_WIDTH);
    connect(motorInfo,SIGNAL(motorChanged(QString)),this,SLOT(motorChanged(QString)));
    connect(motorInfo,SIGNAL(deviceTypeChanged(QString)),this,SLOT(deviceTypeChanged(QString)));
    pSpliterMain->addWidget(motorInfo);

    QSplitter *pRightSpliter = new QSplitter(Qt::Vertical, pSpliterMain);
    pipeSelectHeader = new DevicePipeSelectWidget();
    pipeSelectHeader->setFixedHeight(WINDOW_HEADER_HEIGHT);
    connect(pipeSelectHeader,SIGNAL(pipeChanged(QString,QString,QString,QString)),this,SLOT(selectedPipeChanged(QString,QString,QString,QString)));

    chartInfo = new ChartInfoController(db);
    connect(chartInfo,SIGNAL(openWaveWindow(QString,QString,QString,QString)),this,SLOT(openWaveWindow(QString,QString,QString,QString)));

    pRightSpliter->addWidget(pipeSelectHeader);
    chartInfo->setMinimumSize(800,600);
    pRightSpliter->addWidget(chartInfo);

    pSpliterMain->addWidget(pRightSpliter);

    this->setCentralWidget(pSpliterMain);

    devicelink = new DeviceLinkManager(db,this);
    //QTabWidget *tabWidget = new QTabWidget();

    this->setWindowTitle(tr("mfds"));
    //this->setWindowFlags(Qt::FramelessWindowHint);
    //this->showMaximized();

    QSqlTableModel *settingmodel = db->modelNoHeader("setting");
    int tmpRow = settingmodel->rowCount();

    bool tok;
    int attr_value;
    for(int i=0;i< tmpRow;i++)
    {
        QSqlRecord record = settingmodel->record(i);
        int id = record.value("id").toInt(&tok);
        QString attr_value_s = record.value("attr_value").toString();

        switch (id) {
        case 1:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::com_enable = true;
            }
            else
            {
                MainWindow::com_enable = false;
            }
            break;
        case 2:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::show_enable = true;
            }
            else
            {
                MainWindow::show_enable = false;
            }
            break;
        case 3:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::virtual_data_enable = true;
            }
            else
            {
                MainWindow::virtual_data_enable = false;
            }
            break;
        case 4:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::desktop_capture_enable = true;
            }
            else
            {
                MainWindow::desktop_capture_enable = false;
            }
            break;
        case 5:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::wave_sample_interval = attr_value;
            break;
        case 6:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::freq_sample_interval = attr_value;
            break;
        case 7:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::measure_sample_interval = attr_value;
            break;
        case 8:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::sample_waiting = attr_value;
            break;
        case 9:
            MainWindow::server_ip = attr_value_s;
            break;
        case 10:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::server_waiting = attr_value;
            break;
        case 11:
            MainWindow::group_name = attr_value_s;
            break;
        case 12:
            attr_value = attr_value_s.toInt(&tok);
            MainWindow::data_save_days = attr_value;
            break;
        case 13:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::modbus_server_enable = true;
            }
            else
            {
                MainWindow::modbus_server_enable = false;
            }
            break;
        case 14:
            attr_value = attr_value_s.toInt(&tok);
            if (1 == attr_value)
            {
                MainWindow::server_enable = true;
            }
            else
            {
                MainWindow::server_enable = false;
            }
            break;
        default:
            break;
        }
    }
    delete settingmodel;

    QStatusBar *stateBar = this->statusBar();
    serverState = nullptr;
    if(server_enable)
    {
        serverState = new QLabel();
        serverState->setText(tr("Server"));
        stateBar->addPermanentWidget(serverState);
    }

    QMap<QString,DeviceLinkInfo*> maps = devicelink->getDevicesInfo();
    QMap<QString,DeviceLinkInfo*>::const_iterator it;

    for(it=maps.constBegin();it != maps.constEnd();++it)
    {
        DeviceLinkInfo *dli = it.value();
        QLabel *per1 = new QLabel(this);
        per1->setText(dli->deviceName);

        stateBar->addPermanentWidget(per1);
        deviceStates.append(per1);
    }

    saveThread = new SaveSampleThread(db);
    saveThread->start();

    QTimer::singleShot(20*1000, this, SLOT(connectDevice()));
    cur_motor = "";
    cur_devicetype = "E";

    tcpSocket = nullptr;

    if(desktop_capture_enable)
    {
        QTimer::singleShot(100*1000, this, SLOT(timeDesktopInfo()));
        QTimer *cliptimer = new QTimer(this);
        connect(cliptimer, SIGNAL(timeout()), this, SLOT(timeDesktopInfo()));
        cliptimer->start(1000*60*60*2);
    }

    isconnected = false;
    //this->showMinimized();

    //MainWindow::show_enable = false;

    if(virtual_data_enable)
    {
        QTimer *timerVirtualData = new QTimer(this);
        connect(timerVirtualData, SIGNAL(timeout()), this, SLOT(timeVirtualData()));
        timerVirtualData->start(BASE_INTERVAL);
    }
    modbus_buffer.resize(256);
    for(int i=0;i<256;i++)
    {
        modbus_buffer[i] = 0;
    }

    modbusServer = nullptr;
    if(MainWindow::modbus_server_enable)
    {
        modbusServer = new ModbusTCPServer(this,502);
        //检测更新服务器信号
        //connect(server,SIGNAL(updateServer(QString,int)),this,SLOT(updateServer(QString,int)));
        //connect(server,SIGNAL(updateClientList()),this,SLOT(updateClientList()));

        //CreateBtn->setEnabled(false);
        QLabel *modbus = new QLabel(this);
        modbus->setText(tr("modbus"));

        modbus->setAutoFillBackground(true);
        QPalette pal = modbus->palette();
        pal.setColor(QPalette::Window,QColor(Qt::green));
        modbus->setPalette(pal);

        stateBar->addPermanentWidget(modbus);
    }

    uDisk_name = "";
    if(uDisk_enable)
    {
        m_uDisk = new uDisk;
        qApp->installNativeEventFilter(m_uDisk);

        connect(m_uDisk, &uDisk::sigUDiskCome, [=](QString uDiskName){
            qDebug() << uDiskName << " U Disk Come!";
            uDisk_flag = true;
            if(uDisk_name.length() == 0)
            {
                uDisk_name = uDiskName;
                QTimer::singleShot(10*1000, this, SLOT(uDiskCopyFile()));
            }
        });
        connect(m_uDisk, &uDisk::sigUDiskRemove, [=](){
            qDebug() << "U Disk Remove!";
            uDisk_flag = false;
            uDisk_name = "";
        });
    }
}

void MainWindow::timeVirtualData()
{
    static int count = 0;

    QDateTime current_time = QDateTime::currentDateTime();
    QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");
    if ((count % 2) == 0)
    {
        QByteArray sendBuffer;
        sendBuffer.resize(6);
        sendBuffer[0] = 0;
        sendBuffer[1] = 0xFF;
        sendBuffer[2] = 0xFF;
        sendBuffer[3] = 0;
        sendBuffer[4] = 0;
        sendBuffer[5] = 7;
        QByteArray dataBuffer;

        TemperatureInfo tInfo;
        tInfo.temp = 0.5;
        tInfo.dcode = "d-test";
        tInfo.mcode = "m-test";
        tInfo.pipe = "1";
        tInfo.rksj = StrCurrentTime;

        QString cinfo_str = tInfo.toString() + ";";
        dataBuffer.append(cinfo_str.toUtf8());

        if(dataBuffer.size() > 0)
        {
            QByteArray compress_dataBuffer = qCompress(dataBuffer);
            int compress_data_len = compress_dataBuffer.size();
            sendBuffer[3] = compress_data_len / 0xFF;
            sendBuffer[4] = compress_data_len % 0xFF;
            sendBuffer.append(compress_dataBuffer);
            MainWindow::trans_queue.enqueue(sendBuffer);
        }
    }
    count++;
    if ((count % 7) == 0)
    {
        QByteArray sendBuffer;
        sendBuffer.resize(6);
        sendBuffer[0] = 0;
        sendBuffer[1] = 0xFF;
        sendBuffer[2] = 0xFF;
        sendBuffer[3] = 0;
        sendBuffer[4] = 0;
        sendBuffer[5] = 5;
        QByteArray dataBuffer;

        QString buffer = "";
        for(int i=0;i<1024;i++)
        {
            buffer += QString::number(i) + ",";
        }
        int blen = buffer.length();
        if (blen > 0)
        {
            buffer = buffer.left(blen-1);
        }
        for(int i=0;i<6;i++)
        {
            FreqInfo *fInfo = new FreqInfo();
            fInfo->sample_freq = buffer;
            fInfo->dcode = "d-test";
            fInfo->mcode = "m-test";
            fInfo->rksj = StrCurrentTime;
            fInfo->pipe = "1";

            QString cinfo_str = fInfo->toString() + ";";
            dataBuffer.append(cinfo_str.toUtf8());

            delete fInfo;
        }

        if(dataBuffer.size() > 0)
        {
            QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
            int compress_data_len = compress_dataBuffer.size();
            sendBuffer[3] = compress_data_len / 0xFF;
            sendBuffer[4] = compress_data_len % 0xFF;
            sendBuffer.append(compress_dataBuffer);
            MainWindow::trans_queue.enqueue(sendBuffer);
        }
    }

    if ((count % 40) == 0)
    {
        QString buffer = "";
        for(int i=0;i<16384;i++)
        {
            int sf = i / 10;
            buffer += QString::number(sf) + ",";
        }
        int blen = buffer.length();
        if (blen > 0)
        {
            buffer = buffer.left(blen-1);
        }

        for(int i=0;i<6;i++)
        {
            QByteArray sendBuffer;
            sendBuffer.resize(6);
            sendBuffer[0] = 0;
            sendBuffer[1] = 0xFF;
            sendBuffer[2] = 0xFF;
            sendBuffer[3] = 0;
            sendBuffer[4] = 0;
            sendBuffer[5] = 6;
            QByteArray dataBuffer;

            WaveInfo *wInfo = new WaveInfo();
            wInfo->sample_data = buffer;
            wInfo->dcode = "d-test";
            wInfo->mcode = "m-test";
            wInfo->rksj = StrCurrentTime;
            wInfo->pipe = "1";

            QString cinfo_str = wInfo->toString();
            dataBuffer.append(cinfo_str.toUtf8());

            delete wInfo;

            if(dataBuffer.size() > 0)
            {
                QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                int compress_data_len = compress_dataBuffer.size();
                sendBuffer[3] = compress_data_len / 0xFF;
                sendBuffer[4] = compress_data_len % 0xFF;
                sendBuffer.append(compress_dataBuffer);
                MainWindow::trans_queue.enqueue(sendBuffer);
            }
        }
    }
}

void MainWindow::timeDesktopInfo()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap fullPixmap = screen->grabWindow(0);
    int width = fullPixmap.width();
    int heigth = fullPixmap.height();

    const QPixmap clipMap = fullPixmap.copy(width/4,heigth/4,width/4,heigth/4);

    QImage img = clipMap.toImage();
    //img.toImageFormat(QImage::Format_Mono);
    //img.save("D:/ss.bmp","bmp");
    //QPixmap monoImg = QPixmap::fromImage(img,Qt::ImageConversionFlag::MonoOnly);
    //monoImg.save("D:/ss.jpeg");
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);

    //pixmap不能为空，必须先将图片加载到pixmap中
    img.save(&buffer,"bmp");
    QByteArray pixArray;
    pixArray.append(buffer.data());

    QByteArray sendBuffer;
    sendBuffer.resize(6);
    sendBuffer[0] = 0;
    sendBuffer[1] = 0xFF;
    sendBuffer[2] = 0xFF;
    sendBuffer[3] = 0;
    sendBuffer[4] = 0;
    sendBuffer[5] = 0;
    if (pixArray.size()>0)
    {
        QByteArray compress_dataBuffer = qCompress(pixArray);
        int compress_data_len = compress_dataBuffer.size();
        sendBuffer[3] = compress_data_len / 0xFF;
        sendBuffer[4] = compress_data_len % 0xFF;
        sendBuffer.append(compress_dataBuffer);
        MainWindow::trans_queue_pri.enqueue(sendBuffer);
    }
}

void MainWindow::timeSendInfo()
{
    static int count = 0;
    static int modbus_heart = 0;

    QMap<QString,DeviceLinkInfo*> maps = devicelink->getDevicesInfo();

    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for (it=maps.constBegin(); it != maps.constEnd(); ++it)
    {
        DeviceLinkInfo *dli = it.value();
        QString name = dli->deviceName;
        bool state = dli->linkState;
        int ll = deviceStates.count();
        for(int i=0;i<ll;i++)
        {
            QLabel *pql = deviceStates.at(i);
            if(name == pql->text())
            {
                if (state)
                {
                    pql->setAutoFillBackground(true);
                    QPalette pal = pql->palette();
                    pal.setColor(QPalette::Window,QColor(Qt::green));
                    pql->setPalette(pal);
                }
                else
                {
                    pql->setAutoFillBackground(true);
                    QPalette pal = pql->palette();
                    pal.setColor(QPalette::Window,QColor(Qt::red));
                    pql->setPalette(pal);
                }
            }
        }
    }

    modbus_heart++;
    if(modbus_heart>65535)
    {
        modbus_heart=0;
    }
    modbus_buffer[192] = (char)(modbus_heart / 256);
    modbus_buffer[193] = (char)(modbus_heart % 256);

    if(!server_enable)
    {
        if (MainWindow::trans_queue.size()>0) {
            MainWindow::trans_queue.clear();
        }
        if (MainWindow::trans_queue_pri.size()>0) {
            MainWindow::trans_queue_pri.clear();
        }
        return;
    }
    if (!isconnected)
    {
        if (MainWindow::trans_queue.size()>0) {
            MainWindow::trans_queue.clear();
        }
        if (MainWindow::trans_queue_pri.size()>0)
        {
            MainWindow::trans_queue_pri.clear();
        }
        if (count > MainWindow::server_waiting * 2)
        {
            if(nullptr != tcpSocket)
            {
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
                delete tcpSocket;
                tcpSocket = nullptr;
            }

            tcpSocket = new QTcpSocket(this);
            //检测链接信号
            connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
            //检测如果断开
            connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
            //检测如果有新可以读信号
            connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

            tcpSocket->connectToHost(serverIP,port);
            count = 0;
            isWaiting = false;
        }
        count++;

        if(nullptr != serverState)
        {
            serverState->setAutoFillBackground(true);
            QPalette pal = serverState->palette();
            pal.setColor(QPalette::Window,QColor(Qt::red));
            serverState->setPalette(pal);
        }

        return;
    }

    if(nullptr != serverState)
    {
        serverState->setAutoFillBackground(true);
        QPalette pal = serverState->palette();
        pal.setColor(QPalette::Window,QColor(Qt::green));
        serverState->setPalette(pal);
    }

    if ((MainWindow::trans_queue.size()>0 || MainWindow::trans_queue_pri.size()>0) && !isWaiting) {
        QByteArray msg;
        if (MainWindow::trans_queue_pri.size() > 0)
        {
            msg = MainWindow::trans_queue_pri.dequeue();
            while(trans_queue.size()>MAX_QUEUE_NUM*2)
            {
                trans_queue.dequeue();
            }
        }
        else
        {
            msg = MainWindow::trans_queue.dequeue();
        }
        //tcpSocket->write(msg.toLatin1(),msg.length())

        int slen = tcpSocket->write(msg);
        if (slen < msg.size())
        {
            qDebug() << "send failed";
        }
        else
        {
            tcpSocket->flush();
        }

        isWaiting = true;
    }
    else
    {

        if(isWaiting)
        {
            if (count > MainWindow::server_waiting * 2)
            {
                tcpSocket->disconnectFromHost();
                tcpSocket->close();
                delete tcpSocket;
                tcpSocket = nullptr;

                count = 0;
                isWaiting = false;
            }
            count++;
        }
    }
}

void MainWindow::slotConnected()
{
    isconnected = true;
}

void MainWindow::slotDisconnected()
{
    isconnected = false;
}

void MainWindow::dataReceived()
{
    isWaiting = false;
}

void MainWindow::connectDevice()
{
    //sendThread = new SendDataThread();
    //sendThread->start();

    devicelink->connectDevice();

    serverIP = MainWindow::server_ip;
    //58.20.39.220
    //serverIP = "127.0.0.1";
    //serverIP = "10.7.151.183";
    port = 65134;
    tcpSocket = new QTcpSocket(this);
    //检测链接信号
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(slotConnected()));
    //检测如果断开
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisconnected()));
    //检测如果有新可以读信号
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));

    tcpSocket->connectToHost(serverIP,port);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeSendInfo()));
    timer->start(BASE_INTERVAL/2);

    QByteArray sendBuffer;
    sendBuffer.resize(6);
    sendBuffer[0] = 0;
    sendBuffer[1] = 0xFF;
    sendBuffer[2] = 0xFF;
    sendBuffer[3] = 0;
    sendBuffer[4] = 0;
    sendBuffer[5] = 101;
    if (MainWindow::group_name.length()>0)
    {
        QByteArray compress_dataBuffer = qCompress(MainWindow::group_name.toUtf8());
        int compress_data_len = compress_dataBuffer.size();
        sendBuffer[3] = compress_data_len / 0xFF;
        sendBuffer[4] = compress_data_len % 0xFF;
        sendBuffer.append(compress_dataBuffer);
        MainWindow::trans_queue_pri.enqueue(sendBuffer);
    }
}

void MainWindow::createtables()
{
    QString sql = "CREATE TABLE if not exists setting (id integer NOT NULL PRIMARY KEY,attr_value varchar(50) NOT NULL, description varchar(500))";
    db->createtable(sql);

    sql = "select * from setting where id=1";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (1,'1','com_enable')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=2";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (2,'1','show_enable')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=3";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (3,'0','virtual_data_enable')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=4";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (4,'0','desktop_capture_enable')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=5";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (5,'1800','wave_sample_interval')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=6";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (6,'10','freq_sample_interval')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=7";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (7,'2','measure_sample_interval')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=8";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (8,'30','sample_waiting')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=9";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (9,'127.0.0.1','server_ip')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=10";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (10,'50','server_waiting')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=11";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (11,'default deployment','group name')";
        db->updatasql(sql);
    }

    sql = "select * from setting where id=12";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (12,'3','data save days')";
        db->updatasql(sql);
    }
    sql = "select * from setting where id=13";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (13,'0','modbus server state')";
        db->updatasql(sql);
    }
    sql = "select * from setting where id=14";
    if (db->queryCount(sql) <= 0)
    {
        sql = "insert into setting (id,attr_value,description) values (14,'0','server state')";
        db->updatasql(sql);
    }

    sql = "CREATE TABLE if not exists bearingtype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NOT NULL, btype varchar(50) NOT NULL, rin real NOT NULL, rout real NOT NULL, contact_angle real NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists device (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, code varchar(50) NULL, name varchar(50) NULL, dmodel varchar(50) NULL, dtype varchar(50) NULL, ipaddress varchar(50) NULL, port integer NULL, isactive INTEGER NOT NULL  DEFAULT 1)";
    db->createtable(sql);

    sql = "CREATE TABLE devicetype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NULL, dtype varchar(50) NULL, pipenum integer NOT NULL  DEFAULT (1), description varchar(2048) NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists devicepipes (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, dcode varchar(50) NULL, pipeCode integer NULL, name varchar(50) NULL, motor varchar(50) NULL, isactive INTEGER NOT NULL  DEFAULT 1)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electriccharge (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, u real NULL,i real NULL, f real NULL, factor real NULL, p real NULL, q real NULL, s real NULL, others varchar(100) NULL, pqs varchar(100) NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electricchargewave (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_data text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists electricchargewavefreq (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_freq text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motor (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, mcode varchar(50) NOT NULL, name varchar(50) NOT NULL, motor_type varchar(50) NOT NULL, bearing_type varchar(50) NOT NULL,manufacture_date varchar(50) NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motorgroup (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, name varchar(100) NOT NULL, address varchar(200) NULL, macAddress varchar(100) NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists motortype (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, model varchar(50) NOT NULL, work_mode varchar(50) NOT NULL, power_rating real NULL, rated_voltage real NULL, rated_current real NULL, poleNums integer NULL, center_height real NULL, factor real NULL, insulate integer NULL, rotate real NULL)";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists temperature (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, temp real NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibrate (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, vibrate_e real NULL, speed_e real NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibratewave (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_data text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);

    sql = "CREATE TABLE if not exists vibratewavefreq (id integer NOT NULL PRIMARY KEY AUTOINCREMENT, sample_freq text NULL, dcode varchar(50) NULL, mcode varchar(50) NOT NULL, pipe integer NULL, rksj datetime NOT NULL, stype integer NULL,wid integer NOT NULL default(0))";
    db->createtable(sql);
}

void MainWindow::motormanager()
{
    //qDebug() << "motor" << endl;
    MotorManage *motor = new MotorManage(db,this);
    motor->setAttribute(Qt::WA_DeleteOnClose);
    motor->exec();
}

void MainWindow::motortypemanager()
{
    MotorType *motortype = new MotorType(db);
    motortype->setAttribute(Qt::WA_DeleteOnClose);
    motortype->exec();
}

void MainWindow::bearmanager()
{
    BearType *beartype = new BearType(db);
    beartype->setAttribute(Qt::WA_DeleteOnClose);
    beartype->exec();
}

void MainWindow::devicemanager()
{
    DeviceManager *device = new DeviceManager(db);
    device->setAttribute(Qt::WA_DeleteOnClose);
    device->exec();
}

void MainWindow::devicetypemanager()
{
    DeviceType *type = new DeviceType(db);
    type->setAttribute(Qt::WA_DeleteOnClose);
    type->exec();
}

void MainWindow::devicepipemanager()
{
    DevicePipe *pipe = new DevicePipe(db);
    pipe->setAttribute(Qt::WA_DeleteOnClose);
    pipe->exec();
}

void MainWindow::motorChanged(QString motorCode)
{
    cur_motor = motorCode;

    //QList<DeviceLinkInfo *> devices;
    QMap<QString,DeviceLinkInfo*> maps = devicelink->getDevicesInfo();

    QList<DevicePipeInfo*> pipes;

    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for (it=maps.constBegin(); it != maps.constEnd(); ++it)
    {
        DeviceLinkInfo *dli = it.value();
        if (dli->deviceType == cur_devicetype)
        {
            QList<DevicePipeInfo*>::const_iterator pipeIt;
            for (pipeIt=dli->pipes.constBegin(); pipeIt != dli->pipes.constEnd(); ++pipeIt)
            {
                DevicePipeInfo *dpi = (*pipeIt);
                if(cur_motor == dpi->getMCode())
                {
                    pipes.append(dpi);
                }
            }
        }
    }
    pipeSelectHeader->setupPipes(pipes);
}

void MainWindow::deviceTypeChanged(QString deviceType)
{
    cur_devicetype = deviceType;

    if (cur_motor.length() == 0)
    {
        return;
    }
    QMap<QString,DeviceLinkInfo*> maps = devicelink->getDevicesInfo();

    QList<DevicePipeInfo*> pipes;

    QMap<QString,DeviceLinkInfo*>::const_iterator it;
    for (it=maps.constBegin(); it != maps.constEnd(); ++it)
    {
        DeviceLinkInfo *dli = it.value();
        if (dli->deviceType == cur_devicetype)
        {
            QList<DevicePipeInfo*>::const_iterator pipeIt;
            for (pipeIt=dli->pipes.constBegin(); pipeIt != dli->pipes.constEnd(); ++pipeIt)
            {
                DevicePipeInfo *dpi = (*pipeIt);
                if(cur_motor == dpi->getMCode())
                {
                    pipes.append(dpi);
                }
            }
        }
    }
    pipeSelectHeader->setupPipes(pipes);
}

void MainWindow::selectedPipeChanged(QString motor,QString device,QString pipe,QString deviceType)
{
    qDebug() << "pipe changed" << motor << device << pipe;
    chartInfo->setCurDevicePipe(motor,device,pipe,deviceType);
}

void MainWindow::paramSetting()
{
    ParamSetting *setting = new ParamSetting(db);
    setting->setAttribute(Qt::WA_DeleteOnClose);
    setting->exec();
}

void MainWindow::deviceComSetting()
{
    ComSetting *setting = new ComSetting();

    QMap<QString,DeviceLinkInfo*> maps = devicelink->getDevicesInfo();
    setting->setupDevice(maps);
    setting->setAttribute(Qt::WA_DeleteOnClose);
    setting->exec();
}

void MainWindow::uDiskCopyFile()
{
    if(uDisk_flag)
    {
        qDebug() << "copy database file";
        QString cur_path = QCoreApplication::applicationDirPath();
        //QString dbfile = "D:/Program Files/mfds/data_mf.db";
        QString dbfile = cur_path + "/data_mf.db";

        QDateTime current_time = QDateTime::currentDateTime();
        QString StrCurrentTime = current_time.toString("yyyy-MM-dd");

        QString toFile = uDisk_name + ":/data_" + StrCurrentTime + ".db";
        if(QFile::copy(dbfile,toFile))
        {
            qDebug()<<"copy file succese!";
        }
    }
}

MainWindow::~MainWindow()
{
    saveThread->terminate();
    saveThread->wait();

    //sendThread->terminate();
    //sendThread->wait();

    int len = deviceStates.count();
    for(int i=0;i<len;i++)
    {
        QLabel *tmp = deviceStates.at(i);
        delete tmp;
    }
    deviceStates.clear();

    delete serverState;

    if(nullptr != tcpSocket)
    {
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
        delete tcpSocket;
        tcpSocket = nullptr;
    }

    db->close();
    delete db;

    if(nullptr != modbusServer)
    {
        modbusServer->closeServer();
        delete modbusServer;
    }

    if(m_uDisk) {
        delete m_uDisk;
        m_uDisk = Q_NULLPTR;
    }
}

void MainWindow::openWaveWindow(QString mcode,QString dcode,QString pipe,QString deviceType)
{
    qDebug() << "open wave window";
    WaveForm* wf = new WaveForm(db);
    wf->setAttribute(Qt::WA_DeleteOnClose);
    wf->exec();
}
