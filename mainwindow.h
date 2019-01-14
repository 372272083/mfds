#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QQueue>
#include "devicelinkmanager.h"
#include "savesamplethread.h"

class SqliteDB;
class DeviceLinkManager;
class MotorInfoWidget;
class DevicePipeSelectWidget;
class ChartInfoController;
class ChargeInfo;
class VibrateInfo;
class FreqInfo;
class WaveInfo;
class QLabel;
class TemperatureInfo;
class ModbusTCPServer;
class uDisk;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool com_enable; // id 1
    static bool show_enable; // id 2

    static bool virtual_data_enable; // id 3
    static bool desktop_capture_enable; // id 4

    static bool modbus_server_enable; // id 13
    static bool server_enable; // id 14

    static int wave_sample_interval; // id 5
    static int freq_sample_interval; // id 6
    static int measure_sample_interval; // id 7

    static int sample_waiting; // id 8
    static QString server_ip; // id 9
    static int server_waiting; // id 10

    static QString group_name; // id 11
    static int data_save_days; // id 12

    static int cmie_v_freq_len;
    static int cmie_v_wave_len;

    static int cmie_e_freq_len;
    static int cmie_e_wave_len;

    static QMap<QString,QQueue<ChargeInfo*>> charges;
    static QMap<QString,QQueue<VibrateInfo*>> vibrates;
    static QMap<QString,QQueue<TemperatureInfo*>> temperatures;

    static QMap<QString,QQueue<FreqInfo*>> freqs;
    static QMap<QString,QQueue<WaveInfo*>> waves;

    static QMap<QString,QQueue<FreqInfo*>> freqs_v;
    static QMap<QString,QQueue<WaveInfo*>> waves_v;

    static QQueue<QByteArray> trans_queue;
    static QQueue<QByteArray> trans_queue_pri;

    static QByteArray modbus_buffer;

private:
    SqliteDB *db;
    DeviceLinkManager *devicelink;
    MotorInfoWidget *motorInfo;
    DevicePipeSelectWidget *pipeSelectHeader;
    ChartInfoController *chartInfo;

    QString cur_motor;
    QString cur_devicetype;

    SaveSampleThread* saveThread;

    void createtables();

    bool isconnected;
    bool isWaiting;
    int port;
    QString serverIP;
    QTcpSocket *tcpSocket;

    QList<QLabel*> deviceStates;
    QLabel* serverState;

    ModbusTCPServer* modbusServer;

    uDisk* m_uDisk;
    bool uDisk_enable;
    bool uDisk_flag;
    QString uDisk_name;
public slots:
    void motormanager();
    void motortypemanager();
    void bearmanager();
    void devicemanager();
    void devicetypemanager();
    void devicepipemanager();
    void connectDevice();

    void motorChanged(QString motorCode);
    void deviceTypeChanged(QString deviceType);

    void selectedPipeChanged(QString,QString,QString,QString);

    void timeSendInfo();
    void timeDesktopInfo();
    void timeVirtualData();

    void slotConnected();
    void slotDisconnected();
    void dataReceived();

    void paramSetting();
    void deviceComSetting();

    void uDiskCopyFile();

    void openWaveWindow(QString mcode,QString dcode,QString pipe,QString deviceType);
};

#endif // MAINWINDOW_H
