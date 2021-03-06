#include "cmievdevicelinkinfo.h"
#include "constants.h"

#include "mainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

#include "vibrateinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"

CMIEVDeviceLinkInfo::CMIEVDeviceLinkInfo(QObject *parent) : DeviceLinkInfo(parent),syncTomerOk(false),comReadOk(false),factorReadOk(false)
{
    transFactor.ratio_x1 = 1.0;
    transFactor.ratio_y1 = 1.0;
    transFactor.ratio_z1 = 1.0;

    transFactor.ratio_x2 = 1.0;
    transFactor.ratio_y2 = 1.0;
    transFactor.ratio_z2 = 1.0;

    transFactor.zero_x1 = 0;
    transFactor.zero_y1 = 0;
    transFactor.zero_z1 = 0;

    transFactor.zero_x2 = 0;
    transFactor.zero_y2 = 0;
    transFactor.zero_z2 = 0;

    comSettingParams.ip_h = 0;
    comSettingParams.ip_l = 0;
}

void CMIEVDeviceLinkInfo::init()
{
    syncTomerOk = false;
    comReadOk = false;
    factorReadOk = false;

    DeviceLinkInfo::init();
}

void CMIEVDeviceLinkInfo::handlerReceiveMsg()
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

    unsigned char header1 = rec_buffer[0];
    unsigned char header2 = rec_buffer[1];
    if (header1 == 0xFF && header2 == 0xFF)
    {
        if (datalen == MainWindow::cmie_v_freq_len) //freq
        {
            qDebug() << "freq data";
            int len = (datalen-8)/6;
            for(int i=0; i<6; i++)
            {
                QString buffer;
                int startIndex = len*i+6;
                int endIndex = startIndex + len;
                for(int n=startIndex;n<endIndex;n+=2)
                {
                    QByteArray value_buffer;
                    value_buffer.resize(2);
                    value_buffer[0] = rec_buffer[n];
                    value_buffer[1] = rec_buffer[n + 1];

                    ushort valuedata;
                    memcpy(&valuedata,value_buffer,2);
                    float value_d_f = (float)valuedata;
                    value_d_f *= 0.01;
                    buffer += QString::number(value_d_f,10,4) + ",";
                }

                int blen = buffer.length();
                if (blen > 0)
                {
                    buffer = buffer.left(blen-1);
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

                    FreqInfo *fInfo = new FreqInfo();
                    fInfo->sample_freq = buffer;
                    fInfo->dcode = deviceCode;
                    fInfo->mcode = cur_pipe_motor;
                    fInfo->pipe = cur_pipe;
                    fInfo->stype = 0;
                    fInfo->rksj = StrCurrentTime;

                    if (MainWindow::freqs_v.contains(fInfo->pipe))
                    {
                        if(MainWindow::freqs_v[fInfo->pipe].size()<MAX_QUEUE_NUM * 2)
                        {
                            MainWindow::freqs_v[fInfo->pipe].enqueue(fInfo);
                        }
                    }
                    else
                    {
                        QQueue<FreqInfo*> fqueue;
                        fqueue.enqueue(fInfo);
                        MainWindow::freqs_v[fInfo->pipe] = fqueue;
                    }
                    //MainWindow::freqs_v.enqueue(fInfo);
                }
            }
        }
        else if (datalen == MainWindow::cmie_v_wave_len)
        {
            qDebug() << "wave data:";
            int len = (datalen-8)/6;
            for(int i=0; i<6; i++)
            {
                QString buffer;
                int startIndex = len*i+6;
                int endIndex = startIndex + len;
                for(int n=startIndex;n<endIndex;n+=2)
                {
                    QByteArray value_buffer;
                    value_buffer.resize(2);
                    value_buffer[0] = rec_buffer[n];
                    value_buffer[1] = rec_buffer[n + 1];

                    ushort valuedata;
                    memcpy(&valuedata,value_buffer,2);

                    float fzero = 0.0,fratio = 1.0;
                    switch(i)
                    {
                    case 0:
                        fzero = transFactor.zero_x1;
                        fratio = transFactor.ratio_x1;
                        break;
                    case 1:
                        fzero = transFactor.zero_y1;
                        fratio = transFactor.ratio_y1;
                        break;
                    case 2:
                        fzero = transFactor.zero_z1;
                        fratio = transFactor.ratio_z1;
                        break;
                    case 3:
                        fzero = transFactor.zero_x2;
                        fratio = transFactor.ratio_x2;
                        break;
                    case 4:
                        fzero = transFactor.zero_y2;
                        fratio = transFactor.ratio_y2;
                        break;
                    case 5:
                        fzero = transFactor.zero_z2;
                        fratio = transFactor.ratio_z2;
                        break;
                    }
                    float valuedata_f = (valuedata - fzero)/fratio;

                    buffer += QString::number(valuedata_f,10,4) + ",";
                }

                int blen = buffer.length();
                if (blen > 0)
                {
                    buffer = buffer.left(blen-1);
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

                    WaveInfo *wInfo = new WaveInfo();
                    wInfo->sample_data = buffer;
                    wInfo->dcode = deviceCode;
                    wInfo->mcode = cur_pipe_motor;
                    wInfo->rksj = StrCurrentTime;

                    wInfo->pipe = cur_pipe;
                    wInfo->stype = 0;

                    if (MainWindow::waves_v.contains(wInfo->pipe))
                    {
                        if(MainWindow::waves_v[wInfo->pipe].size()<MAX_QUEUE_NUM)
                        {
                            MainWindow::waves_v[wInfo->pipe].enqueue(wInfo);
                        }
                    }
                    else
                    {
                        QQueue<WaveInfo*> fqueue;
                        fqueue.enqueue(wInfo);
                        MainWindow::waves_v[wInfo->pipe] = fqueue;
                    }
                    //MainWindow::waves_v.enqueue(wInfo);
                }
            }
        }
        isReceiving = false;
        DeviceLinkInfo::handlerReceiveMsg();
        return;
    }

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
        if (unit == TIME_SYNC_W) //time sync ok
        {
            qDebug() << "time sync!";
            struct ModbusTCPMapInfo cmd_save;
            cmd_save.Unit = SAVE_W;
            cmd_save.Addr = 32;
            cmd_save.Command = 0x10;
            cmd_save.Length = 2;
            cmd_save.data = new char[2];
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
        else if(unit == E_FACTOR_R) //read factor ok
        {
            qDebug() << "factor read!";
            factorReadOk = true;
            byteStep = 4;
            for(int i=0; i<12; i++)
            {
                QByteArray factor_buffer;
                factor_buffer.resize(4);
                factor_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);

                float factor = DeviceLinkInfo::bufferTofloat(factor_buffer,HH_HL_LH_LL);

                if (qAbs(factor) < 0.00001)
                {
                    if (i<6)
                    {
                        factor = 0;
                    }
                    else
                    {
                        factor = 1.0;
                    }
                }
                switch(i)
                {
                case 0:
                    transFactor.zero_x1 = factor;
                    break;
                case 1:
                    transFactor.zero_y1 = factor;
                    break;
                case 2:
                    transFactor.zero_z1 = factor;
                    break;
                case 3:
                    transFactor.zero_x2 = factor;
                    break;
                case 4:
                    transFactor.zero_y2 = factor;
                    break;
                case 5:
                    transFactor.zero_z2 = factor;
                    break;
                case 6:
                    transFactor.ratio_x1 = factor;
                    break;
                case 7:
                    transFactor.ratio_y1 = factor;
                    break;
                case 8:
                    transFactor.ratio_z1 = factor;
                    break;
                case 9:
                    transFactor.ratio_x2 = factor;
                    break;
                case 10:
                    transFactor.ratio_y2 = factor;
                    break;
                case 11:
                    transFactor.ratio_z2 = factor;
                    break;
                }
            }
        }
        else if(unit == MEASURE_R) //read measure data ok
        {
            qDebug() << "measure read!";
            byteStep = 4;
            for(int i=0; i<6; i++)
            {
                QByteArray measure_buffer;
                measure_buffer.resize(4);
                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);

                if(MainWindow::modbus_server_enable)
                {
                    MainWindow::modbus_buffer[(16+i)*byteStep] = measure_buffer[0];
                    MainWindow::modbus_buffer[(16+i)*byteStep+1] = measure_buffer[1];
                    MainWindow::modbus_buffer[(16+i)*byteStep+2] = measure_buffer[2];
                    MainWindow::modbus_buffer[(16+i)*byteStep+3] = measure_buffer[3];
                }

                float measure_e = DeviceLinkInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+6)*byteStep,4);

                if(MainWindow::modbus_server_enable)
                {
                    MainWindow::modbus_buffer[(22+i)*byteStep] = measure_buffer[0];
                    MainWindow::modbus_buffer[(22+i)*byteStep+1] = measure_buffer[1];
                    MainWindow::modbus_buffer[(22+i)*byteStep+2] = measure_buffer[2];
                    MainWindow::modbus_buffer[(22+i)*byteStep+3] = measure_buffer[3];
                }

                float measure_s = DeviceLinkInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

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

                    VibrateInfo *vInfo = new VibrateInfo();
                    vInfo->vibrate_e = measure_e;
                    vInfo->speed_e = measure_s;

                    vInfo->dcode = deviceCode;
                    vInfo->mcode = cur_pipe_motor;
                    vInfo->pipe = cur_pipe;
                    vInfo->rksj = StrCurrentTime;

                    if (MainWindow::vibrates.contains(vInfo->pipe))
                    {
                        if(MainWindow::vibrates[vInfo->pipe].size()<MAX_QUEUE_NUM)
                        {
                            MainWindow::vibrates[vInfo->pipe].enqueue(vInfo);
                        }
                    }
                    else
                    {
                        QQueue<VibrateInfo*> fqueue;
                        fqueue.enqueue(vInfo);
                        MainWindow::vibrates[vInfo->pipe] = fqueue;
                    }
                    //MainWindow::vibrates.enqueue(vInfo);
                }
            }
        }
        else if(unit == STATE_R)
        {
            qDebug() << "state read!";
            byteStep = 2;
            QByteArray state_buffer;
            state_buffer.resize(2);
            state_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 1*byteStep];
            state_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 1*byteStep + 1];

            ushort statedata;
            memcpy(&statedata,state_buffer,2);

            int state_wave = statedata & 0x1;
            int state_freq = statedata & 0x2;

            if (state_wave == 1)
            {
                qDebug() << "wave read!";
                struct ModbusTCPMapInfo cmd_wave;
                cmd_wave.Unit = NONE;
                cmd_wave.Addr = 0;
                cmd_wave.Command = 0x0;
                cmd_wave.Length = 5;
                cmd_wave.ExpectLen = MainWindow::cmie_v_wave_len;
                cmd_wave.data = new char[5];

                cmd_wave.data[0] = 87;
                cmd_wave.data[1] = 97;
                cmd_wave.data[2] = 118;
                cmd_wave.data[3] = 101;
                cmd_wave.data[4] = 49;
                msgPriSendQueue.enqueue(cmd_wave);
            }
            else if(state_freq == 2)
            {
                qDebug() << "freq read!";
                struct ModbusTCPMapInfo cmd_freq;
                cmd_freq.Unit = NONE;
                cmd_freq.Addr = 0;
                cmd_freq.Command = 0x0;
                cmd_freq.Length = 5;
                cmd_freq.ExpectLen = MainWindow::cmie_v_freq_len;
                cmd_freq.data = new char[5];

                cmd_freq.data[0] = 87;
                cmd_freq.data[1] = 97;
                cmd_freq.data[2] = 118;
                cmd_freq.data[3] = 101;
                cmd_freq.data[4] = 50;
                msgPriSendQueue.enqueue(cmd_freq);
            }
        }
    }

    isReceiving = false;
    DeviceLinkInfo::handlerReceiveMsg();
}

QString CMIEVDeviceLinkInfo::getComAddress()
{
    unsigned char ip_h_h = comSettingParams.ip_h / 256;
    unsigned char ip_h_l = comSettingParams.ip_h % 256;

    unsigned char ip_l_h = comSettingParams.ip_l / 256;
    unsigned char ip_l_l = comSettingParams.ip_l % 256;

    QString address = QString::number(ip_h_h) + "." + QString::number(ip_h_l) + "." + QString::number(ip_l_h) + "." + QString::number(ip_l_l);
    return address;
}

void CMIEVDeviceLinkInfo::handleSendMsg()
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
    if (!factorReadOk)
    {
        struct ModbusTCPMapInfo cmd_factor;
        cmd_factor.Unit = E_FACTOR_R;
        cmd_factor.Addr = 188;
        cmd_factor.Command = 0x3;
        cmd_factor.Length = 24;
        cmd_factor.ExpectLen = cmd_factor.Length * 2 + 9;
        msgPriSendQueue.enqueue(cmd_factor);
        factorReadOk = true;
        return;
    }

    if (!syncTomerOk || !comReadOk || !factorReadOk)
    {
        return;
    }

    count++;
    int wavewindow = count % MainWindow::wave_sample_interval;
    if (0 == wavewindow)
    {
        struct ModbusTCPMapInfo cmd_wave;
        cmd_wave.Unit = FREQ_W;
        cmd_wave.Addr = 33;
        cmd_wave.Command = 0x10;
        cmd_wave.Length = 2;
        cmd_wave.data = new char[2];
        cmd_wave.data[0] = 0x0;
        cmd_wave.data[1] = 0x1;
        cmd_wave.ExpectLen = 15;
        msgPriSendQueue.enqueue(cmd_wave);
        //return;
    }

    int freqwindow = count % MainWindow::freq_sample_interval;
    if (0 == freqwindow)
    {
        struct ModbusTCPMapInfo cmd_freq;
        cmd_freq.Unit = FREQ_W;
        cmd_freq.Addr = 33;
        cmd_freq.Command = 0x10;
        cmd_freq.Length = 2;
        cmd_freq.data = new char[2];
        cmd_freq.data[0] = 0x0;
        cmd_freq.data[1] = 0x2;
        cmd_freq.ExpectLen = 15;
        msgPriSendQueue.enqueue(cmd_freq);
        //return;
    }

    int subwindow = count % MainWindow::measure_sample_interval;
    if (0 == subwindow)
    {
        struct ModbusTCPMapInfo cmd;
        cmd.Unit = MEASURE_R;
        cmd.Addr = 128;
        cmd.Command = 0x3;
        cmd.Length = 24;
        cmd.ExpectLen = cmd.Length * 2 + 9;
        msgSendQueue.enqueue(cmd);

        struct ModbusTCPMapInfo cmd_state;
        cmd_state.Unit = STATE_R;
        cmd_state.Addr = 50;
        cmd_state.Command = 0x3;
        cmd_state.Length = 5;
        cmd_state.ExpectLen = cmd_state.Length * 2 + 9;
        msgSendQueue.enqueue(cmd_state);
    }
}

QByteArray CMIEVDeviceLinkInfo::getSendMsg()
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
