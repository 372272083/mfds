#include "cmievdeviceinfo.h"

#include "constants.h"

#include "globalvariable.h"

#include <QDateTime>
#include <QMap>
#include <QDebug>
#include <QSqlTableModel>

#include "vibrateinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"
#include "conditioninfo.h"
#include "motorconditiontable.h"

#include "cmievanalysethread.h"
#include "cmievwaveinfo.h"

CMIEVDeviceInfo::CMIEVDeviceInfo(DeviceInfo *parent) : DeviceInfo(parent),syncTomerOk(false),comReadOk(false),factorReadOk(false)
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

    run_mode = 0;
    rotate = 0;
}

void CMIEVDeviceInfo::init()
{
    syncTomerOk = false;
    comReadOk = false;
    factorReadOk = false;
    modelWriteOk = false;
    modelReadOk = false;

    DeviceInfo::init();
}

void CMIEVDeviceInfo::handlerReceiveMsg()
{
    int datalen = rec_buffer.size();
    int slen = rec_buffer.size() - MODBUS_HEAD_LEHGHT;
    int byteStep = 2;
    char cmd;
    char unit;
    static int count_index = 0;

    waitingCount = 0;

    for(int i=0;i<analyse_threads.size();i++)
    {
        CMIEVAnalyseThread* cat = analyse_threads[i];
        if(!cat->isAlive())
        {
            analyse_threads.removeAt(i);
            delete cat;
            break;
        }
    }

    count_index++;
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

    unsigned char header1 = rec_buffer[0];
    unsigned char header2 = rec_buffer[1];
    if (header1 == 0xFF && header2 == 0xFF)
    {
        if (datalen == GlobalVariable::cmie_v_freq_len) //freq
        {
            //qDebug() << "freq data";
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
                    QMutexLocker m_lock(&GlobalVariable::freqsglobalMutex);
                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    FreqInfo* fInfo = new FreqInfo();
                    fInfo->sample_freq = buffer;
                    fInfo->dcode = deviceCode;
                    fInfo->mcode = "";
                    fInfo->pipe = cur_pipe;
                    fInfo->stype = 0;
                    fInfo->rksj = StrCurrentTime;

                    if(GlobalVariable::freqs.contains(deviceCode))
                    {
                        if (GlobalVariable::freqs[deviceCode].contains(fInfo->pipe))
                        {
                            if(GlobalVariable::freqs[deviceCode][fInfo->pipe].size()<MAX_QUEUE_NUM * 2)
                            {
                                GlobalVariable::freqs[deviceCode][fInfo->pipe].enqueue(fInfo);
                            }
                        }
                        else
                        {
                            QQueue<FreqInfo*> fqueue;
                            fqueue.enqueue(fInfo);
                            GlobalVariable::freqs[deviceCode][fInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<FreqInfo*>> dfqueue;
                        QQueue<FreqInfo*> fqueue;
                        fqueue.enqueue(fInfo);
                        dfqueue[fInfo->pipe] = fqueue;

                        GlobalVariable::freqs[deviceCode] = dfqueue;
                    }
                }
            }
        }
        else if (datalen == GlobalVariable::cmie_v_wave_len)
        {
            //qDebug() << "wave read: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
            int len = (datalen-8)/6;

            //DOUBLE_VCT vAccWave[6];
            QMap<int,std::vector<double>> vAccWave;
            QDateTime current_time = QDateTime::currentDateTime();
            QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);
            qDebug() << "receive wave time: " << StrCurrentTime;
            bool record_flag = false;
            if(GlobalVariable::recordwave[deviceCode] > 0)
            {
                record_flag = true;
            }
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
                    //vAccWave[i].push_back(valuedata_f);
                    if(vAccWave.contains(i))
                    {
                        vAccWave[i].push_back(valuedata_f);
                    }
                    else
                    {
                        std::vector<double> st;
                        st.push_back(valuedata_f);
                        vAccWave[i] = st;
                    }

                    buffer += QString::number(valuedata_f,10,4) + ",";
                }

                int blen = buffer.length();
                if (blen > 0)
                {
                    buffer = buffer.left(blen-1);
                }

                QString cur_pipe = QString::number(i+1);

                if(deviceCode.length() > 0)
                {
                    WaveInfo *wInfo = new WaveInfo();
                    wInfo->sample_data = buffer;
                    wInfo->dcode = deviceCode;
                    wInfo->mcode = "";
                    wInfo->rksj = StrCurrentTime;
                    wInfo->pipe = cur_pipe;
                    wInfo->stype = 0;

                    if(GlobalVariable::is_sync_done && record_flag)
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

                        QString cinfo_str = wInfo->toString();
                        dataBuffer.append(cinfo_str.toUtf8());

                        if(dataBuffer.size() > 0)
                        {
                            QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                            int compress_data_len = compress_dataBuffer.size();
                            sendBuffer[3] = compress_data_len / 0xFF;
                            sendBuffer[4] = compress_data_len % 0xFF;
                            sendBuffer.append(compress_dataBuffer);
                            GlobalVariable::trans_queue_pri.enqueue(sendBuffer);
                        }
                    }
                    QMutexLocker m_lock(&GlobalVariable::wavesglobalMutex);
                    if(GlobalVariable::waves.contains(deviceCode))
                    {
                        if (GlobalVariable::waves[deviceCode].contains(wInfo->pipe))
                        {
                            if(GlobalVariable::waves[deviceCode][wInfo->pipe].size()<MAX_QUEUE_NUM)
                            {
                                GlobalVariable::waves[deviceCode][wInfo->pipe].enqueue(wInfo);
                            }
                            /*
                            else
                            {
                                WaveInfo* tmp = GlobalVariable::waves[deviceCode][wInfo->pipe].dequeue();
                                delete tmp;
                            }
                            */
                        }
                        else
                        {
                            QQueue<WaveInfo*> fqueue;
                            fqueue.enqueue(wInfo);
                            GlobalVariable::waves[deviceCode][wInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<WaveInfo*>> dwqueue;
                        QQueue<WaveInfo*> wqueue;
                        wqueue.enqueue(wInfo);
                        dwqueue[wInfo->pipe] = wqueue;

                        GlobalVariable::waves[deviceCode] = dwqueue;
                    }
                }
            }

            if(record_flag)
            {
                int rw = GlobalVariable::recordwave[deviceCode];
                GlobalVariable::recordwave[deviceCode] = rw - 1;
            }

            CMIEVWaveInfo* wi = new CMIEVWaveInfo(vAccWave);
            wi->dcode = deviceCode;
            wi->sample_interval = intervals[0];
            wi->sample_num = samples[0];
            wi->sample_time = StrCurrentTime;
            wi->run_mode = run_mode;

            if(wi->sample_num > 0)
            {
                CMIEVAnalyseThread *wiat = new CMIEVAnalyseThread();
                wiat->setData(wi);
                analyse_threads.push_back(wiat);
                wiat->start();
            }
            else
            {
                delete wi;
            }
        }
        isReceiving = false;
        DeviceInfo::handlerReceiveMsg();
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

            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = COM_R;
            cmd_com.Addr = 0;
            cmd_com.Command = 0x3;
            cmd_com.Length = 23;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
        }
        else if(unit == ROTATE_R) //read setting param ok
        {
            byteStep = 2;

            QByteArray com_buffer;
            com_buffer.resize(2);
            int i=0;
            com_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep];
            com_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + i*byteStep + 1];

            ushort comdata;
            memcpy(&comdata,com_buffer,2);

            float r_tmp = comdata;
            if(qAbs(r_tmp - rotate) > 0.0001)
            {
                rotate = r_tmp;
                ConditionInfo condition;
                condition.dcode = this->deviceCode;
                condition.sample = samples[0];
                condition.interval = intervals[0];
                condition.rotate = rotate;

                QDateTime current_time = QDateTime::currentDateTime();
                QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                condition.rksj = StrCurrentTime;

                //GlobalVariable::conditions.enqueue(condition);

                if(GlobalVariable::conditions.contains(deviceCode))
                {
                    GlobalVariable::conditions[deviceCode].enqueue(condition);
                }
                else
                {
                    QQueue<ConditionInfo> fqueue;
                    fqueue.enqueue(condition);
                    GlobalVariable::conditions[deviceCode] = fqueue;
                }
            }
        }
        else if(unit == COM_R) //read setting param ok
        {
            qDebug() << "com reading!";
            samples.clear();
            intervals.clear();

            comReadOk = true;
            byteStep = 2;
            for(int i=0; i<23; i++)
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
                case 11:
                    ratio.ratio_1 = comdata * PC_RATIO;
                    break;
                case 12:
                    ratio.ratio_2 = comdata * PC_RATIO;
                    break;
                case 13:
                    ratio.ratio_3 = comdata * PC_RATIO;
                    break;
                case 14:
                    ratio.ratio_4 = comdata * PC_RATIO;
                    break;
                case 15:
                    ratio.ratio_5 = comdata * PC_RATIO;
                    break;
                case 16:
                    ratio.ratio_6 = comdata * PC_RATIO;
                    break;
                case 17:
                    samples.append(comdata);
                    break;
                case 18:
                    intervals.append(comdata);
                    break;
                case 19:
                    filter_freqency_l_s.append(comdata);
                    break;
                case 20:
                    filter_freqency_h_s.append(comdata);
                    break;
                case 21:
                    break;
                case 22:
                    break;
                }
            }
            if(samples.size()>0 && intervals.size()>0)
            {
                ConditionInfo condition;
                condition.dcode = this->deviceCode;
                condition.sample = samples[0];
                condition.interval = intervals[0];
                condition.rotate = rotate;

                QDateTime current_time = QDateTime::currentDateTime();
                QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                condition.rksj = StrCurrentTime;

                //GlobalVariable::conditions.enqueue(condition);

                if(GlobalVariable::conditions.contains(deviceCode))
                {
                    GlobalVariable::conditions[deviceCode].enqueue(condition);
                }
                else
                {
                    QQueue<ConditionInfo> fqueue;
                    fqueue.enqueue(condition);
                    GlobalVariable::conditions[deviceCode] = fqueue;
                }
            }
            else
            {
                samples.append(16384);
                intervals.clear();
                intervals.append(1);
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

                float factor = DeviceInfo::bufferTofloat(factor_buffer,HH_HL_LH_LL);

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
            qDebug() << "factor test";
        }
        else if(unit == MODE_V_W)
        {
            modelWriteOk = true;
            //run_mode = 1;

            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = MODE_V_R;
            cmd_com.Addr = 36;
            cmd_com.Command = 0x3;
            cmd_com.Length = 1;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
        }
        else if(unit == MODE_V_R)
        {
            byteStep = 2;
            modelReadOk = true;
            QByteArray com_buffer;
            com_buffer.resize(2);
            com_buffer[1] = rec_buffer[MODBUS_HEAD_LEHGHT + 0*byteStep];
            com_buffer[0] = rec_buffer[MODBUS_HEAD_LEHGHT + 0*byteStep + 1];

            ushort comdata;
            memcpy(&comdata,com_buffer,2);
            if(comdata == 4)
            {
                run_mode = 1;
                //modelWriteOk = true;
            }
            else
            {
                run_mode = 0;
                //modelWriteOk = false;
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

                if(GlobalVariable::modbus_server_enable)
                {
                    GlobalVariable::modbus_buffer[(16+i)*byteStep] = measure_buffer[0];
                    GlobalVariable::modbus_buffer[(16+i)*byteStep+1] = measure_buffer[1];
                    GlobalVariable::modbus_buffer[(16+i)*byteStep+2] = measure_buffer[2];
                    GlobalVariable::modbus_buffer[(16+i)*byteStep+3] = measure_buffer[3];
                }

                float measure_e = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+6)*byteStep,4);

                if(GlobalVariable::modbus_server_enable)
                {
                    GlobalVariable::modbus_buffer[(22+i)*byteStep] = measure_buffer[0];
                    GlobalVariable::modbus_buffer[(22+i)*byteStep+1] = measure_buffer[1];
                    GlobalVariable::modbus_buffer[(22+i)*byteStep+2] = measure_buffer[2];
                    GlobalVariable::modbus_buffer[(22+i)*byteStep+3] = measure_buffer[3];
                }

                float measure_s = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+12)*byteStep,4);

                if(GlobalVariable::modbus_server_enable)
                {
                    GlobalVariable::modbus_buffer[(28+i)*byteStep] = measure_buffer[0];
                    GlobalVariable::modbus_buffer[(28+i)*byteStep+1] = measure_buffer[1];
                    GlobalVariable::modbus_buffer[(28+i)*byteStep+2] = measure_buffer[2];
                    GlobalVariable::modbus_buffer[(28+i)*byteStep+3] = measure_buffer[3];
                }

                float measure_p = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

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

                if(deviceCode.length() > 0)
                {
                    QMutexLocker m_lock(&GlobalVariable::vibratesglobalMutex);
                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    VibrateInfo *vInfo = new VibrateInfo();
                    vInfo->vibrate_e = measure_e;
                    vInfo->speed_e = measure_s;
                    vInfo->position_e = measure_p;
                    vInfo->dcode = deviceCode;
                    vInfo->pipe = cur_pipe;
                    vInfo->rksj = StrCurrentTime;

                    if(GlobalVariable::vibrates.contains(deviceCode))
                    {
                        if (GlobalVariable::vibrates[deviceCode].contains(vInfo->pipe))
                        {
                            if(GlobalVariable::vibrates[deviceCode][vInfo->pipe].size()<MAX_QUEUE_NUM)
                            {
                                GlobalVariable::vibrates[deviceCode][vInfo->pipe].enqueue(vInfo);
                            }
                            /*
                            else
                            {
                                VibrateInfo* tmp = GlobalVariable::vibrates[deviceCode][vInfo->pipe].dequeue();
                                delete tmp;
                            }
                            */
                        }
                        else
                        {
                            QQueue<VibrateInfo*> fqueue;
                            fqueue.enqueue(vInfo);
                            GlobalVariable::vibrates[deviceCode][vInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<VibrateInfo*>> mchqueue;
                        QQueue<VibrateInfo*> chqueue;
                        chqueue.enqueue(vInfo);

                        mchqueue[vInfo->pipe] = chqueue;
                        GlobalVariable::vibrates[deviceCode] = mchqueue;
                    }
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
                //qDebug() << "wave read: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
                struct ModbusTCPMapInfo cmd_wave;
                cmd_wave.Unit = NONE;
                cmd_wave.Addr = 0;
                cmd_wave.Command = 0x0;
                cmd_wave.Length = 5;
                cmd_wave.ExpectLen = GlobalVariable::cmie_v_wave_len;
                cmd_wave.data = new unsigned char[5];

                cmd_wave.data[0] = 87;
                cmd_wave.data[1] = 97;
                cmd_wave.data[2] = 118;
                cmd_wave.data[3] = 101;
                cmd_wave.data[4] = 49;
                msgPriSendQueue.enqueue(cmd_wave);
            }
            else if(state_freq == 2)
            {
                /*
                qDebug() << "freq read!";
                struct ModbusTCPMapInfo cmd_freq;
                cmd_freq.Unit = NONE;
                cmd_freq.Addr = 0;
                cmd_freq.Command = 0x0;
                cmd_freq.Length = 5;
                cmd_freq.ExpectLen = GlobalVariable::cmie_v_freq_len;
                cmd_freq.data = new unsigned char[5];

                cmd_freq.data[0] = 87;
                cmd_freq.data[1] = 97;
                cmd_freq.data[2] = 118;
                cmd_freq.data[3] = 101;
                cmd_freq.data[4] = 50;
                msgPriSendQueue.enqueue(cmd_freq);
                */
            }
        }
    }

    isReceiving = false;
    DeviceInfo::handlerReceiveMsg();
}

QString CMIEVDeviceInfo::getComAddress()
{
    unsigned char ip_h_h = comSettingParams.ip_h / 256;
    unsigned char ip_h_l = comSettingParams.ip_h % 256;

    unsigned char ip_l_h = comSettingParams.ip_l / 256;
    unsigned char ip_l_l = comSettingParams.ip_l % 256;

    QString address = QString::number(ip_h_h) + "." + QString::number(ip_h_l) + "." + QString::number(ip_l_h) + "." + QString::number(ip_l_l);
    return address;
}

void CMIEVDeviceInfo::handleSendMsg()
{
    static int count = 1; //multi-windows

    //qDebug()<<"Send Msg: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
    //qDebug()<<"Send Msgs Counter: " << QString::number(msgPriSendQueue.size());
    //qDebug()<<"Wave Msg: " << (isHaveSomeState(1,NONE)? "1":"0");

    if(msgPriSendQueue.size()>MAX_QUEUE_NUM || msgSendQueue.size()>MAX_QUEUE_NUM)
    {
        return;
    }

    if (isReceiving)
    {
        if((GlobalVariable::s_t == 10 || (GlobalVariable::s_t == 0 && GlobalVariable::s_t_sub_v == 10)) && !isHaveSomeState(1,NONE))
        {
            if (syncTomerOk && comReadOk && factorReadOk && modelReadOk && modelWriteOk)
            {
                struct ModbusTCPMapInfo cmd_wave;
                cmd_wave.Unit = NONE;
                cmd_wave.Num = 0;
                cmd_wave.Addr = 0;
                cmd_wave.Command = 0x0;
                cmd_wave.Length = 5;
                cmd_wave.ExpectLen = GlobalVariable::cmie_v_wave_len;
                cmd_wave.data = new unsigned char[5];

                cmd_wave.data[0] = 87;
                cmd_wave.data[1] = 97;
                cmd_wave.data[2] = 118;
                cmd_wave.data[3] = 101;
                cmd_wave.data[4] = 49;
                msgPriSendQueue.enqueue(cmd_wave);
            }
        }

        //qDebug()<<"Receiving: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
        return;
    }
    if (!syncTomerOk)
    {
        /*
        struct ModbusTCPMapInfo cmd_t;
        cmd_t.Unit = TIME_SYNC_W;
        cmd_t.Addr = 39;
        cmd_t.Command = 0x10;
        cmd_t.Length = 4;
        cmd_t.data = new unsigned char[4];
        cmd_t.ExpectLen = 15;
        msgPriSendQueue.enqueue(cmd_t);
        */
        syncTomerOk = true;
        //return;
    }
    if (!modelReadOk)
    {
        if(!isHaveSomeState(1,MODE_V_R))
        {
            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = MODE_V_R;
            cmd_com.Num = 0;
            cmd_com.Addr = 36;
            cmd_com.Command = 0x3;
            cmd_com.Length = 1;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
            //modelReadOk = true;
        }
        return;
    }
    if (!comReadOk)
    {
        if(!isHaveSomeState(1,COM_R))
        {
            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = COM_R;
            cmd_com.Num = 0;
            cmd_com.Addr = 0;
            cmd_com.Command = 0x3;
            cmd_com.Length = 23;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
            //comReadOk = true;
        }
        return;
    }
    if((GlobalVariable::s_t == 10  || (GlobalVariable::s_t == 0 && GlobalVariable::s_t_sub_v == 10))&& run_mode == 0)
    {
        if (!modelWriteOk)
        {
            if(!isHaveSomeState(1,MODE_V_W))
            {
                struct ModbusTCPMapInfo cmd_mode;
                cmd_mode.Unit = MODE_V_W;
                cmd_mode.Num = 0;
                cmd_mode.Addr = 36;
                cmd_mode.Command = 0x10;
                cmd_mode.Length = 2;
                cmd_mode.data = new unsigned char[2];
                cmd_mode.data[0] = 0x0;
                cmd_mode.data[1] = 0x4;
                cmd_mode.ExpectLen = 15;
                msgPriSendQueue.enqueue(cmd_mode);
            }

            //modelWriteOk = true;
            return;
        }
    }
    else if(GlobalVariable::s_t != 10 && run_mode == 1)
    {
        if(GlobalVariable::s_t == 0 && GlobalVariable::s_t_sub_v != 10)
        {
            if (!modelWriteOk)
            {
                if(!isHaveSomeState(1,MODE_V_W))
                {
                    struct ModbusTCPMapInfo cmd_mode;
                    cmd_mode.Unit = MODE_V_W;
                    cmd_mode.Num = 0;
                    cmd_mode.Addr = 36;
                    cmd_mode.Command = 0x10;
                    cmd_mode.Length = 2;
                    cmd_mode.data = new unsigned char[2];
                    cmd_mode.data[0] = 0x0;
                    cmd_mode.data[1] = 0x0;
                    cmd_mode.ExpectLen = 15;
                    msgPriSendQueue.enqueue(cmd_mode);
                }
                //modelWriteOk = true;
                return;
            }
        }
    }
    if (!factorReadOk)
    {
        if(!isHaveSomeState(1,E_FACTOR_R))
        {
            struct ModbusTCPMapInfo cmd_factor;
            cmd_factor.Unit = E_FACTOR_R;
            cmd_factor.Num = 0;
            if(run_mode == 1)
            {
                cmd_factor.Addr = 360;
            }
            else
            {
                cmd_factor.Addr = 188;//360;
            }
            cmd_factor.Command = 0x3;
            cmd_factor.Length = 24;
            cmd_factor.ExpectLen = cmd_factor.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_factor);
            //factorReadOk = true;
        }
        return;
    }

    if (!syncTomerOk || !comReadOk || !factorReadOk || !modelWriteOk || !modelReadOk)
    {
        return;
    }

    count++;
    if(0 == run_mode)
    {
        int wavewindow = count % (wave_interval*2);
        if (0 == wavewindow)
        {
            if(!isHaveSomeState(1,FREQ_W))
            {
                struct ModbusTCPMapInfo cmd_wave;
                cmd_wave.Unit = FREQ_W;
                cmd_wave.Num = 0;
                cmd_wave.Addr = 33;
                cmd_wave.Command = 0x10;
                cmd_wave.Length = 2;
                cmd_wave.data = new unsigned char[2];
                cmd_wave.data[0] = 0x0;
                cmd_wave.data[1] = 0x1;
                cmd_wave.ExpectLen = 15;
                msgPriSendQueue.enqueue(cmd_wave);
                //return;

                qDebug()<<"Record wave time: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);
            }
        }

        /*
        int freqwindow = count % (freq_interval*2);
        if (0 == freqwindow)
        {
            if(!isHaveSomeState(1,FREQ_W_W))
            {
                struct ModbusTCPMapInfo cmd_freq;
                cmd_freq.Unit = FREQ_W_W;
                cmd_freq.Addr = 33;
                cmd_freq.Command = 0x10;
                cmd_freq.Length = 2;
                cmd_freq.data = new unsigned char[2];
                cmd_freq.data[0] = 0x0;
                cmd_freq.data[1] = 0x2;
                cmd_freq.ExpectLen = 15;
                msgPriSendQueue.enqueue(cmd_freq);
            }

            //return;
        }
        */
    }

    if(0 == run_mode)
    {
        int subwindow = count % (measure_interval*2);
        if (0 == subwindow)
        {
            if(!isHaveSomeState(0,MEASURE_R))
            {
                struct ModbusTCPMapInfo cmd;
                cmd.Unit = MEASURE_R;
                cmd.Num = 0;
                cmd.Addr = 128;
                cmd.Command = 0x3;
                cmd.Length = 36;
                cmd.ExpectLen = cmd.Length * 2 + 9;
                msgSendQueue.enqueue(cmd);
            }

            if(!isHaveSomeState(0,STATE_R))
            {
                struct ModbusTCPMapInfo cmd_state;
                cmd_state.Unit = STATE_R;
                cmd_state.Num = 0;
                cmd_state.Addr = 50;
                cmd_state.Command = 0x3;
                cmd_state.Length = 5;
                cmd_state.ExpectLen = cmd_state.Length * 2 + 9;
                msgSendQueue.enqueue(cmd_state);
            }

            if(!isHaveSomeState(0,ROTATE_R))
            {
                struct ModbusTCPMapInfo cmd_com;
                cmd_com.Unit = ROTATE_R;
                cmd_com.Num = 0;
                cmd_com.Addr = 23;
                cmd_com.Command = 0x3;
                cmd_com.Length = 1;
                cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
                msgSendQueue.enqueue(cmd_com);
            }
        }
    }
    else
    {
        struct ModbusTCPMapInfo cmd_wave;
        cmd_wave.Unit = NONE;
        cmd_wave.Num = 0;
        cmd_wave.Addr = 0;
        cmd_wave.Command = 0x0;
        cmd_wave.Length = 5;
        cmd_wave.ExpectLen = GlobalVariable::cmie_v_wave_len;
        cmd_wave.data = new unsigned char[5];

        cmd_wave.data[0] = 87;
        cmd_wave.data[1] = 97;
        cmd_wave.data[2] = 118;
        cmd_wave.data[3] = 101;
        cmd_wave.data[4] = 49;
        msgPriSendQueue.enqueue(cmd_wave);
        //qDebug()<<"wave 0x0 cmd: " << QDateTime::currentDateTime().toString(GlobalVariable::dtFormat);

        //QDateTime dt = QDateTime::currentDateTime();
        //qDebug() << dt.toString(GlobalVariable::dtFormat);

        int subwindow = count % 2;

        if(subwindow == 0 && !isHaveSomeState(1,ROTATE_R))
        {
            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = ROTATE_R;
            cmd_com.Num = 0;
            cmd_com.Addr = 23;
            cmd_com.Command = 0x3;
            cmd_com.Length = 1;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
        }
    }
}

void CMIEVDeviceInfo::close()
{
    msgPriSendQueue.clear();
}

QString CMIEVDeviceInfo::getParam(int type)
{
    //type 10: ratio1 ;11 ratio2; 12 ratio3; 13 ratio4; 14 ratio5; 15 ratio6; 16 rotate; 17 samples; 18 sampleinterval;19 mode
    QString result = "";
    switch (type) {
    case 10:
        result = QString::number(ratio.ratio_1);
        break;
    case 11:
        result = QString::number(ratio.ratio_2);
        break;
    case 12:
        result = QString::number(ratio.ratio_3);
        break;
    case 13:
        result = QString::number(ratio.ratio_4);
        break;
    case 14:
        result = QString::number(ratio.ratio_5);
        break;
    case 15:
        result = QString::number(ratio.ratio_6);
        break;
    case 16:
        result = QString::number(rotate,10,4);
        break;
    case 17:
        for(QList<int>::ConstIterator cIt = samples.constBegin();cIt != samples.constEnd();cIt++)
        {
            int v = *cIt;
            result += QString::number(v) + ",";
        }
        break;
    case 18:
        for(QList<float>::ConstIterator cIt = intervals.constBegin();cIt != intervals.constEnd();cIt++)
        {
            float v = *cIt;
            result += QString::number(v,10,1) + ",";
        }
        break;
    case 19:
        result = QString::number(run_mode);
        break;
    case 20:
        for(QList<float>::ConstIterator cIt = filter_freqency_l_s.constBegin();cIt != filter_freqency_l_s.constEnd();cIt++)
        {
            float v = *cIt;
            result += QString::number(v,10,1) + ",";
        }
        break;
    case 21:
        for(QList<float>::ConstIterator cIt = filter_freqency_h_s.constBegin();cIt != filter_freqency_h_s.constEnd();cIt++)
        {
            float v = *cIt;
            result += QString::number(v,10,1) + ",";
        }
        break;
    case 22:
        result = QString::number(rotate,10,1);
        break;
    default:
        break;
    }

    return result;
}
