#include "cmietdevicelinkinfo.h"
#include "constants.h"

#include "mainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

#include "temperatureinfo.h"

CMIETDeviceLinkInfo::CMIETDeviceLinkInfo(QObject *parent) : DeviceLinkInfo(parent)
{
    comSettingParams.ip_h = 0;
    comSettingParams.ip_l = 0;
}

void CMIETDeviceLinkInfo::init()
{
    syncTomerOk = false;
    comReadOk = false;

    DeviceLinkInfo::init();
}

void CMIETDeviceLinkInfo::handlerReceiveMsg()
{
    int datalen = rec_buffer.size();
    int slen = rec_buffer.size() - MODBUS_HEAD_LEHGHT;
    int byteStep = 2;
    char cmd;
    char unit;

    waitingCount = 0;
    //isReceiving = false;
    qDebug() << "receive data lenght: " << datalen;
    if (datalen <=0)
    {
        //failed
        isReceiving = false;
        DeviceLinkInfo::handlerReceiveMsg();
        return;
    }
    if (datalen <= 6)
    {
        //record wave
        isReceiving = false;
        DeviceLinkInfo::handlerReceiveMsg();
        return;
    }
    //handle wave data
    cmd = rec_buffer[7];
    unit = rec_buffer[6];

    if (datalen <=9) //modbus error
    {
        qDebug() << "modbus data error!";
    }
    else
    {
        if (cmd == 0x3)
        {
            qDebug() << "modbus data h3!";
            char mq0 = rec_buffer[0];
            char mq1 = rec_buffer[1];
            char mq2 = rec_buffer[2];
            char mq3 = rec_buffer[3];
            char mq5 = rec_buffer[5];
            char mq8 = rec_buffer[8];
            qDebug() << "modbus data len: " << ((int)mq5);
            if (mq0 == 0x0 && mq1 == 0x0 && mq2 == 0x0 && mq3 == 0x0 && mq5 == (mq8 + 3))
            {

            }
            else //modbus valid error!
            {
                isReceiving = false;
                DeviceLinkInfo::handlerReceiveMsg();
                return;
            }
        }
        else if (cmd == 0x10)
        {
            qDebug() << "modbus data h10!";
        }

        if (unit == TIME_SYNC_W) //time sync ok
        {
            qDebug() << "time sync!";
            syncTomerOk = true;
        }
        else if(unit == COM_R) //read setting param ok
        {
            qDebug() << "com setting!";
            comReadOk = true;
            byteStep = 2;
            for(int i=0; i<10; i++)
            {
                QByteArray com_buffer;
                com_buffer.resize(2);
                com_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep];
                com_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep + 1];

                ushort comdata;
                memcpy(&comdata,com_buffer,2);

                switch(i)
                {
                case 0:
                    comSettingParams.ip_h = comdata;
                    break;
                case 1:
                    comSettingParams.ip_l = comdata;
                    break;
                case 2:
                    comSettingParams.ip_mask_h = comdata;
                    break;
                case 3:
                    comSettingParams.ip_mask_l = comdata;
                    break;
                case 4:
                    comSettingParams.ip_gate_h = comdata;
                    break;
                case 5:
                    comSettingParams.ip_gate_l = comdata;
                    break;
                case 6:
                    comSettingParams.rs485_baud_1 = comdata;
                    break;
                case 7:
                    comSettingParams.rs485_addr_1 = comdata;
                    break;
                case 8:
                    comSettingParams.rs485_baud_2 = comdata;
                    break;
                case 9:
                    comSettingParams.rs485_addr_2 = comdata;
                    break;
                }
            }
        }
        else if(unit == MEASURE_R) //read measure data ok
        {
            qDebug() << "measure read!";
            byteStep = 2;
            for(int i=0; i<12; i++)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(2);
                measure_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep];
                measure_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep + 1];
                //measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,2);

                ushort valuedata;
                memcpy(&valuedata,measure_buffer,2);

                float measure_v = (valuedata - 500)/10;

                if (i < 8 && MainWindow::modbus_server_enable)
                {
                    QByteArray arr = DeviceLinkInfo::floatToBuffer(measure_v,HH_HL_LH_LL);
                    MainWindow::modbus_buffer[(32+i)*byteStep*2] = arr[0];
                    MainWindow::modbus_buffer[(32+i)*byteStep*2+1] = arr[1];
                    MainWindow::modbus_buffer[(32+i)*byteStep*2+2] = arr[2];
                    MainWindow::modbus_buffer[(32+i)*byteStep*2+3] = arr[3];
                }

                QString cur_pipe = QString::number(i+1);
                QString cur_pipe_motor = "";

                QList<DevicePipeInfo*>::const_iterator pipeIt;
                for (pipeIt=pipes.constBegin(); pipeIt != pipes.constEnd(); ++pipeIt)
                {
                    DevicePipeInfo *dpi = (*pipeIt);
                    if(cur_pipe == dpi->getPipeCode())
                    {
                        cur_pipe_motor = dpi->getMCode();
                    }
                }
                if(deviceCode.length() > 0 && cur_pipe_motor.length() > 0)
                {
                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");

                    TemperatureInfo *tInfo = new TemperatureInfo();
                    tInfo->temp = measure_v;
                    tInfo->dcode = deviceCode;
                    tInfo->mcode = cur_pipe_motor;
                    tInfo->pipe = cur_pipe;
                    tInfo->rksj = StrCurrentTime;

                    if (MainWindow::temperatures.contains(tInfo->pipe))
                    {
                        if(MainWindow::temperatures[tInfo->pipe].size()<MAX_QUEUE_NUM)
                        {
                            MainWindow::temperatures[tInfo->pipe].enqueue(tInfo);
                        }
                    }
                    else
                    {
                        QQueue<TemperatureInfo*> fqueue;
                        fqueue.enqueue(tInfo);
                        MainWindow::temperatures[tInfo->pipe] = fqueue;
                    }

                    //MainWindow::temperatures.enqueue(tInfo);
                }
            }
        }
    }

    isReceiving = false;
    DeviceLinkInfo::handlerReceiveMsg();
}

QString CMIETDeviceLinkInfo::getComAddress()
{
    unsigned char ip_h_h = comSettingParams.ip_h / 256;
    unsigned char ip_h_l = comSettingParams.ip_h % 256;

    unsigned char ip_l_h = comSettingParams.ip_l / 256;
    unsigned char ip_l_l = comSettingParams.ip_l % 256;

    QString address = QString::number(ip_h_h) + "." + QString::number(ip_h_l) + "." + QString::number(ip_l_h) + "." + QString::number(ip_l_l);
    return address;
}

void CMIETDeviceLinkInfo::handleSendMsg()
{
    static int count = 0; //multi-windows

    if(msgPriSendQueue.size()>MAX_QUEUE_NUM || msgSendQueue.size()>MAX_QUEUE_NUM)
    {
        return;
    }

    if (isReceiving)
    {
        return;
    }
    if (!syncTomerOk)
    {
        struct ModbusTCPMapInfo cmd_t;
        cmd_t.Unit = TIME_SYNC_W;
        cmd_t.Addr = 39;
        cmd_t.Command = 0x10;
        cmd_t.Length = 4;
        cmd_t.data = new char[4];
        cmd_t.ExpectLen = 15;
        msgPriSendQueue.enqueue(cmd_t);
        syncTomerOk = true;
        return;
    }
    if (!comReadOk)
    {
        struct ModbusTCPMapInfo cmd_com;
        cmd_com.Unit = COM_R;
        cmd_com.Addr = 0;
        cmd_com.Command = 0x3;
        cmd_com.Length = 10;
        cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
        msgPriSendQueue.enqueue(cmd_com);
        comReadOk = true;
        return;
    }

    if (!syncTomerOk || !comReadOk)
    {
        return;
    }

    int subwindow = count % MainWindow::measure_sample_interval;
    if (0 != subwindow)
    {
        struct ModbusTCPMapInfo cmd;
        cmd.Unit = MEASURE_R;
        cmd.Addr = 104;
        cmd.Command = 0x3;
        cmd.Length = 12;
        cmd.ExpectLen = cmd.Length * 2 + 9;
        msgSendQueue.enqueue(cmd);
    }
    count++;
}

QByteArray CMIETDeviceLinkInfo::getSendMsg()
{
    if (isReceiving)
    {
        QByteArray ba;
        return ba;
    }
    isReceiving = true;
    struct ModbusTCPMapInfo cmd;
    if (msgPriSendQueue.count() > 0)
    {
        cmd = msgPriSendQueue.dequeue();
    }
    else if (msgSendQueue.count() > 0)
    {
        cmd = msgSendQueue.dequeue();
    }
    else
    {
        QByteArray ba;
        isReceiving = false;
        return ba;
    }

    expDataLen = cmd.ExpectLen;

    if (cmd.Command == 0x3 || cmd.Command == 0x10)
    {
        if (cmd.Unit == TIME_SYNC_W)
        {
            QDateTime dt = QDateTime::currentDateTime();
            unsigned int seconds = dt.toTime_t();
            unsigned int time_h = seconds / 0x10000;
            unsigned int time_l = seconds % 0x10000;
            cmd.data[0] = time_l / 0xFF;
            cmd.data[1] = time_l % 0xFF;
            cmd.data[2] = time_h / 0xFF;
            cmd.data[3] = time_h % 0xFF;
        }
        QByteArray ba = modbusEncomposeMsg(cmd);
        if (cmd.Command == 0x10)
        {
            if (nullptr != cmd.data)
            {
                delete cmd.data;
            }
        }
        //isReceiving = true;
        return ba;
    }
    else if (cmd.Command == 0x0)
    {
        qDebug() << "command 0x0!";
        QByteArray ba;
        ba.resize(cmd.Length);
        for(int i=0;i<cmd.Length;i++)
        {
            ba[i] = cmd.data[i];
        }
        return ba;
    }
    else
    {
        QByteArray ba;
        isReceiving = false;
        return ba;
    }
}
