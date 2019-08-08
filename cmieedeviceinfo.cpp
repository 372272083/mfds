#include "cmieedeviceinfo.h"

#include "constants.h"

#include "globalvariable.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlTableModel>

#include "chargeinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"

#include "conditioninfo.h"
#include "motorconditiontable.h"

#include "cmieeanalysethread.h"
#include "cmieewaveinfo.h"

CMIEEDeviceInfo::CMIEEDeviceInfo(DeviceInfo *parent) : DeviceInfo(parent)
{
    transFactor.ratio_ca = 1.0;
    transFactor.ratio_cb = 1.0;
    transFactor.ratio_cc = 1.0;

    transFactor.ratio_va = 1.0;
    transFactor.ratio_vb = 1.0;
    transFactor.ratio_vc = 1.0;

    transFactor.zero_ca = 0;
    transFactor.zero_cb = 0;
    transFactor.zero_cc = 0;

    transFactor.zero_va = 0;
    transFactor.zero_vb = 0;
    transFactor.zero_vc = 0;

    comSettingParams.ip_h = 0;
    comSettingParams.ip_l = 0;

    comSettingParams.uar = 1.0;
    comSettingParams.ubr = 1.0;
    comSettingParams.ucr = 1.0;
    comSettingParams.iar = 1.0;
    comSettingParams.ibr = 1.0;
    comSettingParams.icr = 1.0;
}

void CMIEEDeviceInfo::init()
{
    syncTomerOk = false;
    comReadOk = false;
    factorReadOk = false;

    DeviceInfo::init();
}

void CMIEEDeviceInfo::handlerReceiveMsg()
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
        CMIEEAnalyseThread* cat = analyse_threads[i];
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
        if (datalen == GlobalVariable::cmie_e_freq_len) //freq
        {
            qDebug() << "freq data";
            int len = (datalen-8)/6;
            for(int i=0; i<6; i++)
            {
                QString buffer = "";
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
                    float uiar = 1.0;
                    switch(i)
                    {
                    case 0:
                        uiar = comSettingParams.uar * PC_RATIO * 0.01;
                        break;
                    case 1:
                        uiar = comSettingParams.ubr * PC_RATIO * 0.01;
                        break;
                    case 2:
                        uiar = comSettingParams.ucr * PC_RATIO * 0.01;
                        break;
                    case 3:
                        uiar = comSettingParams.iar * PC_RATIO * 0.001;
                        break;
                    case 4:
                        uiar = comSettingParams.ibr * PC_RATIO * 0.001;
                        break;
                    case 5:
                        uiar = comSettingParams.icr * PC_RATIO * 0.001;
                        break;
                    }

                    float value_d_f = (float)valuedata;
                    value_d_f *= uiar;

                    buffer += QString::number(value_d_f,10,4) + ",";
                }
                int blen = buffer.length();
                if (blen > 0)
                {
                    buffer = buffer.left(blen-1);
                }

                QString cur_pipe = "0";
                if (i<3)
                {
                    cur_pipe = QString::number(i+1);
                }
                else
                {
                    cur_pipe = QString::number(i-2);
                }
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

                    FreqInfo* fInfo = new FreqInfo();
                    fInfo->sample_freq = buffer;
                    fInfo->dcode = deviceCode;
                    fInfo->mcode = "";
                    fInfo->rksj = StrCurrentTime;
                    fInfo->pipe = cur_pipe;
                    if (i<3)
                    {
                        fInfo->stype = 0;
                    }
                    else
                    {
                        fInfo->stype = 1;
                    }
                    QMutexLocker m_lock(&GlobalVariable::freqsglobalMutex);
                    if(GlobalVariable::freqs.contains(deviceCode))
                    {
                        if (GlobalVariable::freqs[deviceCode].contains(fInfo->pipe))
                        {
                            if(GlobalVariable::freqs[deviceCode][fInfo->pipe].size()<MAX_QUEUE_NUM*2)
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
        else if (datalen == GlobalVariable::cmie_e_wave_len)
        {
            qDebug() << "wave data:";
            int len = (datalen-8)/6;
            //DOUBLE_VCT vVolWave[3],vCurWave[3];
            QMap<int,std::vector<double>> vVolWave;
            QMap<int,std::vector<double>> vCurWave;
            QDateTime current_time = QDateTime::currentDateTime();
            QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

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

                    float fzero = 0.0,fratio = 1.0,uiar = 1.0;
                    switch(i)
                    {
                    case 0:
                        fzero = transFactor.zero_va;
                        fratio = transFactor.ratio_va;
                        uiar = comSettingParams.uar * PC_RATIO;
                        break;
                    case 1:
                        fzero = transFactor.zero_vb;
                        fratio = transFactor.ratio_vb;
                        uiar = comSettingParams.ubr * PC_RATIO;
                        break;
                    case 2:
                        fzero = transFactor.zero_vc;
                        fratio = transFactor.ratio_vc;
                        uiar = comSettingParams.ucr * PC_RATIO;
                        break;
                    case 3:
                        fzero = transFactor.zero_ca;
                        fratio = transFactor.ratio_ca;
                        uiar = comSettingParams.iar * PC_RATIO;
                        break;
                    case 4:
                        fzero = transFactor.zero_cb;
                        fratio = transFactor.ratio_cb;
                        uiar = comSettingParams.ibr * PC_RATIO;
                        break;
                    case 5:
                        fzero = transFactor.zero_cc;
                        fratio = transFactor.ratio_cc;
                        uiar = comSettingParams.icr * PC_RATIO;
                        break;
                    }
                    float valuedata_f = (valuedata - fzero)/fratio;
                    valuedata_f *= uiar;

                    if (i<3)
                    {
                        //vVolWave[i].push_back(valuedata_f);
                        if(vVolWave.contains(i))
                        {
                            vVolWave[i].push_back(valuedata_f);
                        }
                        else
                        {
                            std::vector<double> st;
                            st.push_back(valuedata_f);
                            vVolWave[i] = st;
                        }
                    }
                    else
                    {
                        //vCurWave[i-3].push_back(valuedata_f);
                        if(vCurWave.contains(i-3))
                        {
                            vCurWave[i-3].push_back(valuedata_f);
                        }
                        else
                        {
                            std::vector<double> st;
                            st.push_back(valuedata_f);
                            vCurWave[i-3] = st;
                        }
                    }

                    buffer += QString::number(valuedata_f,10,4) + ",";
                }

                int blen = buffer.length();
                if (blen > 0)
                {
                    buffer = buffer.left(blen-1);
                }
                //qDebug() << buffer.mid(0,4096);
                QString cur_pipe = "0";
                if (i<3)
                {
                    cur_pipe = QString::number(i+1);
                }
                else
                {
                    cur_pipe = QString::number(i-2);
                }
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
                    WaveInfo* wInfo = new WaveInfo();
                    wInfo->sample_data = buffer;
                    wInfo->dcode = deviceCode;
                    wInfo->mcode = "";
                    wInfo->rksj = StrCurrentTime;
                    wInfo->pipe = cur_pipe;
                    wInfo->is_Continuity = 0;
                    if (i<3)
                    {
                        wInfo->stype = 0;
                    }
                    else
                    {
                        wInfo->stype = 1;
                    }

                    if(GlobalVariable::is_sync_done && record_flag)
                    {
                        QByteArray sendBuffer;
                        sendBuffer.resize(6);
                        sendBuffer[0] = 0;
                        sendBuffer[1] = 0xFF;
                        sendBuffer[2] = 0xFF;
                        sendBuffer[3] = 0;
                        sendBuffer[4] = 0;
                        sendBuffer[5] = 3;
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

            CMIEEWaveInfo* wi = new CMIEEWaveInfo(vVolWave,vCurWave);
            wi->mcode = "";
            QVector<QString> ms = GlobalVariable::findMotorCodeByDeviceCode(deviceCode);
            if(ms.size()>0)
            {
                wi->mcode = ms.at(0);
            }
            wi->dcode = deviceCode;
            wi->sample_interval = intervals[0];
            wi->sample_num = samples[0];
            wi->sample_time = StrCurrentTime;
            wi->run_mode = 0;

            if(wi->sample_num > 0)
            {
                CMIEEAnalyseThread *wiat = new CMIEEAnalyseThread();
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
            cmd_com.Length = 22;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
        }
        else if(unit == COM_R) //read setting param ok
        {
            qDebug() << "com setting!";
            comReadOk = true;
            byteStep = 2;
            intervals.clear();
            samples.clear();
            for(int i=0; i<22; i++)
            {
                QByteArray com_buffer;
                com_buffer.resize(2);
                com_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,2);

                ushort comdata = DeviceInfo::bufferToUShort(com_buffer,HH_LL);

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
                case 10:
                    comSettingParams.data_format = comdata;
                    break;
                case 11:
                    comSettingParams.uar = comdata;
                    break;
                case 12:
                    comSettingParams.ubr = comdata;
                    break;
                case 13:
                    comSettingParams.ucr = comdata;
                    break;
                case 14:
                    comSettingParams.iar = comdata;
                    break;
                case 15:
                    comSettingParams.ibr = comdata;
                    break;
                case 16:
                    comSettingParams.icr = comdata;
                    break;
                case 17:
                    samples.append(comdata);
                    //comSettingParams.wave_num = comdata;
                    break;
                case 18:
                    intervals.append(comdata);
                    //comSettingParams.wave_sample = comdata;
                    break;
                case 19:
                    comSettingParams.update_rate = comdata;
                    break;
                case 20:
                    comSettingParams.wire_choose = comdata;
                    break;
                case 21:
                    comSettingParams.calculate_method = comdata;
                    break;
                }
            }
            if(samples.size()>0 && intervals.size()>0)
            {
                ConditionInfo condition;
                condition.dcode = this->deviceCode;
                condition.sample = samples[0];
                condition.interval = intervals[0];
                condition.rotate = 0;

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

                if(qAbs(factor) < 0.00001)
                {
                    if(i<6)
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
                    transFactor.zero_va = factor;
                    break;
                case 1:
                    transFactor.zero_vb = factor;
                    break;
                case 2:
                    transFactor.zero_vc = factor;
                    break;
                case 3:
                    transFactor.zero_ca = factor;
                    break;
                case 4:
                    transFactor.zero_cb = factor;
                    break;
                case 5:
                    transFactor.zero_cc = factor;
                    break;
                case 6:
                    transFactor.ratio_va = factor;
                    break;
                case 7:
                    transFactor.ratio_vb = factor;
                    break;
                case 8:
                    transFactor.ratio_vc = factor;
                    break;
                case 9:
                    transFactor.ratio_ca = factor;
                    break;
                case 10:
                    transFactor.ratio_cb = factor;
                    break;
                case 11:
                    transFactor.ratio_cc = factor;
                    break;
                }
            }
        }
        else if(unit == MEASURE_R) //read measure data ok
        {
            qDebug() << "measure read!";
            byteStep = 4;
            QByteArray measure_buffer;

            measure_buffer.resize(4);
            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 21*byteStep,4);
            float measure_ua_ub = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 22*byteStep,4);
            float measure_ub_uc = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 23*byteStep,4);
            float measure_uc_ua = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 24*byteStep,4);
            float measure_pabc = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            if(GlobalVariable::modbus_server_enable)
            {
                GlobalVariable::modbus_buffer[8*byteStep] = measure_buffer[0];
                GlobalVariable::modbus_buffer[8*byteStep+1] = measure_buffer[1];
                GlobalVariable::modbus_buffer[8*byteStep+2] = measure_buffer[2];
                GlobalVariable::modbus_buffer[8*byteStep+3] = measure_buffer[3];
            }

            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 25*byteStep,4);
            float measure_qabc = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            if(GlobalVariable::modbus_server_enable)
            {
                GlobalVariable::modbus_buffer[9*byteStep] = measure_buffer[0];
                GlobalVariable::modbus_buffer[9*byteStep+1] = measure_buffer[1];
                GlobalVariable::modbus_buffer[9*byteStep+2] = measure_buffer[2];
                GlobalVariable::modbus_buffer[9*byteStep+3] = measure_buffer[3];
            }

            measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + 26*byteStep,4);
            float measure_sabc = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

            if(GlobalVariable::modbus_server_enable)
            {
                GlobalVariable::modbus_buffer[10*byteStep] = measure_buffer[0];
                GlobalVariable::modbus_buffer[10*byteStep+1] = measure_buffer[1];
                GlobalVariable::modbus_buffer[10*byteStep+2] = measure_buffer[2];
                GlobalVariable::modbus_buffer[10*byteStep+3] = measure_buffer[3];
            }

            QString u_a_b_c = QString::number(measure_ua_ub,10,4) + "," + QString::number(measure_ub_uc,10,4) + "," + QString::number(measure_uc_ua,10,4);
            QString pqs_abc = QString::number(measure_pabc,10,4) + "," + QString::number(measure_qabc,10,4) + "," + QString::number(measure_sabc,10,4);

            for(int i=0; i<3; i++)
            {
                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + i*byteStep,4);
                float measure_u = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);
                /*
                switch (i) {
                case 0:
                    measure_u *= comSettingParams.uar * PC_RATIO;
                    break;
                case 1:
                    measure_u *= comSettingParams.ubr * PC_RATIO;
                    break;
                case 2:
                    measure_u *= comSettingParams.ucr * PC_RATIO;
                    break;
                default:
                    break;
                }
                */

                if(GlobalVariable::modbus_server_enable)
                {
                    QByteArray arr = DeviceInfo::floatToBuffer(measure_u,HH_HL_LH_LL);

                    GlobalVariable::modbus_buffer[i*byteStep] = arr[0];
                    GlobalVariable::modbus_buffer[i*byteStep+1] = arr[1];
                    GlobalVariable::modbus_buffer[i*byteStep+2] = arr[2];
                    GlobalVariable::modbus_buffer[i*byteStep+3] = arr[3];
                }

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+3)*byteStep,4);
                float measure_i = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                /*
                switch (i) {
                case 0:
                    measure_i *= comSettingParams.iar * PC_RATIO;
                    break;
                case 1:
                    measure_i *= comSettingParams.ibr * PC_RATIO;
                    break;
                case 2:
                    measure_i *= comSettingParams.icr * PC_RATIO;
                    break;
                default:
                    break;
                }
                */

                if(GlobalVariable::modbus_server_enable)
                {
                    QByteArray arr = DeviceInfo::floatToBuffer(measure_i,HH_HL_LH_LL);

                    GlobalVariable::modbus_buffer[(i+3)*byteStep] = arr[0];
                    GlobalVariable::modbus_buffer[(i+3)*byteStep+1] = arr[1];
                    GlobalVariable::modbus_buffer[(i+3)*byteStep+2] = arr[2];
                    GlobalVariable::modbus_buffer[(i+3)*byteStep+3] = arr[3];
                }

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+6)*byteStep,4);
                float measure_f = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                if(GlobalVariable::modbus_server_enable)
                {
                    GlobalVariable::modbus_buffer[(6)*byteStep] = measure_buffer[0];
                    GlobalVariable::modbus_buffer[(6)*byteStep+1] = measure_buffer[1];
                    GlobalVariable::modbus_buffer[(6)*byteStep+2] = measure_buffer[2];
                    GlobalVariable::modbus_buffer[(6)*byteStep+3] = measure_buffer[3];
                }

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+9)*byteStep,4);
                float measure_factor = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                if(GlobalVariable::modbus_server_enable)
                {
                    GlobalVariable::modbus_buffer[(7)*byteStep] = measure_buffer[0];
                    GlobalVariable::modbus_buffer[(7)*byteStep+1] = measure_buffer[1];
                    GlobalVariable::modbus_buffer[(7)*byteStep+2] = measure_buffer[2];
                    GlobalVariable::modbus_buffer[(7)*byteStep+3] = measure_buffer[3];
                }

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+12)*byteStep,4);
                float measure_p = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+15)*byteStep,4);
                float measure_q = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

                measure_buffer = rec_buffer.mid(MODBUS_HEAD_LEHGHT + (i+18)*byteStep,4);
                float measure_s = DeviceInfo::bufferTofloat(measure_buffer,HH_HL_LH_LL);

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
                    ChargeInfo *cInfo = new ChargeInfo();
                    cInfo->u = measure_u;
                    cInfo->i = measure_i;
                    cInfo->f = measure_f;
                    cInfo->factor = measure_factor;
                    cInfo->p = measure_p;
                    cInfo->q = measure_q;
                    cInfo->s = measure_s;
                    cInfo->others = u_a_b_c;
                    cInfo->pqs = pqs_abc;

                    QDateTime current_time = QDateTime::currentDateTime();
                    QString StrCurrentTime = current_time.toString(GlobalVariable::dtFormat);

                    cInfo->dcode = deviceCode;
                    cInfo->pipe = cur_pipe;
                    cInfo->rksj = StrCurrentTime;

                    QMutexLocker m_lock(&GlobalVariable::chargesglobalMutex);
                    if(GlobalVariable::charges.contains(deviceCode))
                    {
                        if (GlobalVariable::charges[deviceCode].contains(cInfo->pipe))
                        {
                            if(GlobalVariable::charges[deviceCode][cInfo->pipe].size()<MAX_QUEUE_NUM)
                            {
                                GlobalVariable::charges[deviceCode][cInfo->pipe].enqueue(cInfo);
                            }
                            /*
                            else
                            {
                                ChargeInfo* tmp = GlobalVariable::charges[deviceCode][vInfo->pipe].dequeue();
                                delete tmp;
                            }
                            */
                        }
                        else
                        {
                            QQueue<ChargeInfo*> fqueue;
                            fqueue.enqueue(cInfo);
                            GlobalVariable::charges[deviceCode][cInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<ChargeInfo*>> mchqueue;
                        QQueue<ChargeInfo*> chqueue;
                        chqueue.enqueue(cInfo);

                        mchqueue[cInfo->pipe] = chqueue;
                        GlobalVariable::charges[deviceCode] = mchqueue;
                    }

                    //MainWindow::charges.enqueue(cInfo);
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

void CMIEEDeviceInfo::handleSendMsg()
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
    if (!comReadOk)
    {
        if(!isHaveSomeState(1,COM_R))
        {
            struct ModbusTCPMapInfo cmd_com;
            cmd_com.Unit = COM_R;
            cmd_com.Num = 0;
            cmd_com.Addr = 0;
            cmd_com.Command = 0x3;
            cmd_com.Length = 22;
            cmd_com.ExpectLen = cmd_com.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_com);
            //comReadOk = true;
        }
        return;
    }
    if (!factorReadOk)
    {
        if(!isHaveSomeState(1,E_FACTOR_R))
        {
            struct ModbusTCPMapInfo cmd_factor;
            cmd_factor.Unit = E_FACTOR_R;
            cmd_factor.Num = 0;
            cmd_factor.Addr = 188;
            cmd_factor.Command = 0x3;
            cmd_factor.Length = 24;
            cmd_factor.ExpectLen = cmd_factor.Length * 2 + 9;
            msgPriSendQueue.enqueue(cmd_factor);
            //factorReadOk = true;
        }
        return;
    }

    if (!syncTomerOk || !comReadOk || !factorReadOk)
    {
        return;
    }

    count++;
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
        }
        //return;
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
            cmd.Length = 56;
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
    }
}

QString CMIEEDeviceInfo::getParam(int type)
{
    QString result = "";
    switch (type) {
    case 10:
        result = QString::number(comSettingParams.uar * PC_RATIO);
        break;
    case 11:
        result = QString::number(comSettingParams.ubr * PC_RATIO);
        break;
    case 12:
        result = QString::number(comSettingParams.ucr * PC_RATIO);
        break;
    case 13:
        result = QString::number(comSettingParams.iar * PC_RATIO);
        break;
    case 14:
        result = QString::number(comSettingParams.ibr * PC_RATIO);
        break;
    case 15:
        result = QString::number(comSettingParams.icr * PC_RATIO);
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
    default:
        break;
    }

    return result;
}

void CMIEEDeviceInfo::close()
{

}

QString CMIEEDeviceInfo::getComAddress()
{
    unsigned char ip_h_h = (unsigned char)(comSettingParams.ip_h / 256);
    unsigned char ip_h_l = (unsigned char)(comSettingParams.ip_h % 256);

    unsigned char ip_l_h = (unsigned char)(comSettingParams.ip_l / 256);
    unsigned char ip_l_l = (unsigned char)(comSettingParams.ip_l % 256);

    QString address = QString::number(ip_h_h) + "." + QString::number(ip_h_l) + "." + QString::number(ip_l_h) + "." + QString::number(ip_l_l);
    return address;
}
