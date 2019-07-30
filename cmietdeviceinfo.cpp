#include "cmietdeviceinfo.h"
#include "constants.h"

#include "globalvariable.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

#include "temperatureinfo.h"

CMIETDeviceInfo::CMIETDeviceInfo(DeviceInfo *parent) : DeviceInfo(parent)
{
    comSettingParams.ip_h = 0;
    comSettingParams.ip_l = 0;
}

void CMIETDeviceInfo::init()
{
    syncTomerOk = false;
    comReadOk = false;

    DeviceInfo::init();
}

void CMIETDeviceInfo::handlerReceiveMsg()
{
    int datalen = rec_buffer.size();
    int slen = rec_buffer.size() - MODBUS_HEAD_LEHGHT;
    int byteStep = 2;
    char cmd;
    char unit;

    waitingCount = 0;
    //isReceiving = false;
    //qDebug() << "receive data lenght: " << datalen;
    if (datalen <=0)
    {
        //failed
        isReceiving = false;
        DeviceInfo::handlerReceiveMsg();
        return;
    }
    if (datalen <= 6)
    {
        //record wave
        isReceiving = false;
        DeviceInfo::handlerReceiveMsg();
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
            //qDebug() << "modbus data h3!";
            char mq0 = rec_buffer[0];
            char mq1 = rec_buffer[1];
            char mq2 = rec_buffer[2];
            char mq3 = rec_buffer[3];
            char mq5 = rec_buffer[5];
            char mq8 = rec_buffer[8];
            //qDebug() << "modbus data len: " << ((int)mq5);
            if (mq0 == 0x0 && mq1 == 0x0 && mq2 == 0x0 && mq3 == 0x0 && mq5 == (mq8 + 3))
            {

            }
            else //modbus valid error!
            {
                isReceiving = false;
                DeviceInfo::handlerReceiveMsg();
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
        if (unit == TIME_SYNC_W) //time sync ok
        {
            qDebug() << "time sync!";
            struct ModbusTCPMapInfo cmd_save;
            cmd_save.Unit = SAVE_W;
            cmd_save.Addr = 32;
            cmd_save.Command = 0x10;
            cmd_save.Length = 2;
            cmd_save.data = new unsigned char[2];
            cmd_save.data[0] = 0x0;
            cmd_save.data[0] = 0x1;
            cmd_save.ExpectLen = 15;
            msgPriSendQueue.enqueue(cmd_save);
            syncTomerOk = true;
        }
        else if(unit == SAVE_W)
        {
            qDebug() << "time sync save!";
        }
        else if(unit == COM_W)
        {
            qDebug() << "com sync save!";
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
                if(i<8 && GlobalVariable::modbus_server_enable)
                {
                    QByteArray arr = DeviceInfo::floatToBuffer(measure_v,HH_HL_LH_LL);
                    GlobalVariable::modbus_buffer[(32+i)*byteStep*2] = arr[0];
                    GlobalVariable::modbus_buffer[(32+i)*byteStep*2+1] = arr[1];
                    GlobalVariable::modbus_buffer[(32+i)*byteStep*2+2] = arr[2];
                    GlobalVariable::modbus_buffer[(32+i)*byteStep*2+3] = arr[3];
                }

                QString cur_pipe = QString::number(i+1);
                /*
                QString cur_pipe_motor = "";

                QList<ChannelInfo*>::const_iterator pipeIt;
                for (pipeIt=pipes.constBegin(); pipeIt != pipes.constEnd(); ++pipeIt)
                {
                    ChannelInfo *dpi = (*pipeIt);
                    if(cur_pipe == dpi->channel_code)
                    {
                        cur_pipe_motor = dpi->motor_code;
                    }
                }
                */
                if(deviceCode.length() > 0/* && cur_pipe_motor.length() > 0*/)
                {
                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    TemperatureInfo *tInfo = new TemperatureInfo();
                    tInfo->temp = measure_v;
                    tInfo->dcode = deviceCode;
                    //tInfo->mcode = cur_pipe_motor;
                    tInfo->pipe = cur_pipe;
                    tInfo->rksj = StrCurrentTime;

                    QMutexLocker m_lock(&GlobalVariable::temperaturesglobalMutex);
                    if(GlobalVariable::temperatures.contains(deviceCode))
                    {
                        if (GlobalVariable::temperatures[deviceCode].contains(tInfo->pipe))
                        {
                            if(GlobalVariable::temperatures[deviceCode][tInfo->pipe].size()<MAX_QUEUE_NUM)
                            {
                                GlobalVariable::temperatures[deviceCode][tInfo->pipe].enqueue(tInfo);
                            }
                        }
                        else
                        {
                            QQueue<TemperatureInfo*> fqueue;
                            fqueue.enqueue(tInfo);
                            GlobalVariable::temperatures[deviceCode][tInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<TemperatureInfo*>> mchqueue;
                        QQueue<TemperatureInfo*> chqueue;
                        chqueue.enqueue(tInfo);

                        mchqueue[tInfo->pipe] = chqueue;
                        GlobalVariable::temperatures[deviceCode] = mchqueue;
                    }
                }
            }
        }
    }

    isReceiving = false;
    DeviceInfo::handlerReceiveMsg();
}

QString CMIETDeviceInfo::getComAddress()
{
    unsigned char ip_h_h = comSettingParams.ip_h / 256;
    unsigned char ip_h_l = comSettingParams.ip_h % 256;

    unsigned char ip_l_h = comSettingParams.ip_l / 256;
    unsigned char ip_l_l = comSettingParams.ip_l % 256;

    QString address = QString::number(ip_h_h) + "." + QString::number(ip_h_l) + "." + QString::number(ip_l_h) + "." + QString::number(ip_l_l);
    return address;
}

void CMIETDeviceInfo::close()
{

}

void CMIETDeviceInfo::handleSendMsg()
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
        cmd_t.data = new unsigned char[4];
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

    int subwindow = count % measure_interval;
    if (0 == subwindow)
    {
        if(!isHaveSomeState(0,MEASURE_R))
        {
            struct ModbusTCPMapInfo cmd;
            cmd.Unit = MEASURE_R;
            cmd.Addr = 104;
            cmd.Command = 0x3;
            cmd.Length = 12;
            cmd.ExpectLen = cmd.Length * 2 + 9;
            msgSendQueue.enqueue(cmd);
        }
    }
    count++;
}
