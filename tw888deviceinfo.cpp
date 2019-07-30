#include "tw888deviceinfo.h"

#include "constants.h"

#include "globalvariable.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

TW888DeviceINfo::TW888DeviceINfo(DeviceInfo *parent) : DeviceInfo(parent)
{
    area_id = 0;
    machine_id = 0;
}

void TW888DeviceINfo::init()
{
    DeviceInfo::init();
}

void TW888DeviceINfo::handlerReceiveMsg()
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

        if(unit == MEASURE_R) //read measure data ok
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

                if(deviceCode.length() > 0/* && cur_pipe_motor.length() > 0*/)
                {
                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);
                }
            }
        }
    }

    isReceiving = false;
    DeviceInfo::handlerReceiveMsg();
}

QString TW888DeviceINfo::getComAddress()
{
    return "";
}

void TW888DeviceINfo::close()
{

}

void TW888DeviceINfo::handleSendMsg()
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
