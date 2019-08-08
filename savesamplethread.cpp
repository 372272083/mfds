#include "savesamplethread.h"

#include <QException>
#include <QSqlTableModel>
#include <QList>

#include "globalvariable.h"
#include "sqlitedb.h"

#include "chargeinfo.h"
#include "tw888info.h"
#include "vibrateinfo.h"
#include "temperatureinfo.h"

#include "freqinfo.h"
#include "waveinfo.h"

#include "conditioninfo.h"
#include "motorconditiontable.h"

#include <QDebug>
#include <QDateTime>

SaveSampleThread::SaveSampleThread(SqliteDB *db):mdb(db),state(false)
{

}

void SaveSampleThread::setState(bool state)
{
    this->state = state;
}

void SaveSampleThread::run()
{
    int row = 0;
    int maxCount = 0;
    int maxLimit = 1;
    QQueue<QString> sqls;
    while(!state)
    {
        //QDateTime start,end;
        //start = QDateTime::currentDateTime();
        sqls.clear();
        try
        {
            maxCount = 0;
            QMutexLocker m_lock(&GlobalVariable::chargesglobalMutex);
            if(GlobalVariable::charges.size() > 0)
            {
                //QSqlTableModel* model = mdb->modelNoHeader("electriccharge");

                QList<QString> keyList = GlobalVariable::charges.keys();

                QList<QString>::const_iterator it,iit;
                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 1;
                QByteArray dataBuffer;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QMap<QString,QQueue<ChargeInfo*>> deviceData = GlobalVariable::charges[*it];
                    QList<QString> inkeyList = deviceData.keys();
                    //save info
                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        while(GlobalVariable::charges[*it][*iit].size() > 1)
                        {
                            ChargeInfo* cinfo = GlobalVariable::charges[*it][*iit].dequeue();

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(cinfo->rksj,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    QString sql = "insert into electriccharge (u,i,f,factor,p,q,s,others,pqs,dcode,mcode,pipe,rksj) values (";
                                    sql += QString::number(cinfo->u) + "," + QString::number(cinfo->i) + "," + QString::number(cinfo->f) + "," + QString::number(cinfo->factor) + "," + QString::number(cinfo->p) + "," + QString::number(cinfo->q) + "," + QString::number(cinfo->s);
                                    sql += ",'" + cinfo->others + "','" + cinfo->pqs + "','" + cinfo->dcode + "','" + cinfo->mcode + "'," + cinfo->pipe + ",'" + cinfo->rksj + "')";
                                    sqls.enqueue(sql);
                                }
                            }

                            int len_factor = GlobalVariable::channelInfos.size() > 0 ? GlobalVariable::channelInfos.size() : 1;
                            if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM*len_factor)
                            {
                                QString cinfo_str = cinfo->toString() + ";";
                                //qDebug() << cinfo_str;
                                dataBuffer.append(cinfo_str.toUtf8());
                            }
                            //qDebug() << "trans queue size: " << QString::number(GlobalVariable::trans_queue.size());
                            if(GlobalVariable::trans_queue.size()>=MAX_QUEUE_NUM*len_factor)
                            {
                                GlobalVariable::trans_queue.pop_front();
                            }

                            delete cinfo;
                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
                if (GlobalVariable::is_sync_done && dataBuffer.size()>0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }
            }

            //if (sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {

        }

        try
        {
            maxCount = 0;
            QMutexLocker m_lock(&GlobalVariable::tw888chargesglobalMutex);
            if(GlobalVariable::tw888charges.size() > 0)
            {
                //QSqlTableModel* model = mdb->modelNoHeader("electriccharge");

                QList<QString> keyList = GlobalVariable::tw888charges.keys();

                QList<QString>::const_iterator it,iit;
                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 1;
                QByteArray dataBuffer;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QQueue<TW888Info*> deviceData = GlobalVariable::tw888charges[*it];

                    while(GlobalVariable::tw888charges[*it].size() > 1)
                    {
                        TW888Info* cinfo = GlobalVariable::tw888charges[*it].dequeue();

                        bool isRwave = false;
                        if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                        {
                            QDateTime dt1 = QDateTime::fromString(cinfo->rksj,GlobalVariable::dtFormat);
                            QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                            int s_diff = dt2.secsTo(dt1);
                            if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                            {
                                isRwave = true;
                            }
                        }

                        if(GlobalVariable::isSaveData || isRwave)
                        {
                            if(GlobalVariable::data_save_days > 0)
                            {
                                /*
                                QString sql = "insert into electriccharge (u,i,f,factor,p,q,s,others,pqs,dcode,mcode,pipe,rksj) values (";
                                sql += QString::number(cinfo->u) + "," + QString::number(cinfo->i) + "," + QString::number(cinfo->f) + "," + QString::number(cinfo->factor) + "," + QString::number(cinfo->p) + "," + QString::number(cinfo->q) + "," + QString::number(cinfo->s);
                                sql += ",'" + cinfo->others + "','" + cinfo->pqs + "','" + cinfo->dcode + "','" + cinfo->mcode + "'," + cinfo->pipe + ",'" + cinfo->rksj + "')";
                                sqls.enqueue(sql);
                                */
                            }
                        }

                        int len_factor = GlobalVariable::channelInfos.size() > 0 ? GlobalVariable::channelInfos.size() : 1;
                        if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM*len_factor)
                        {
                            QString cinfo_str = cinfo->toString() + ";";
                            //qDebug() << cinfo_str;
                            dataBuffer.append(cinfo_str.toUtf8());
                        }
                        //qDebug() << "trans queue size: " << QString::number(GlobalVariable::trans_queue.size());
                        if(GlobalVariable::trans_queue.size()>=MAX_QUEUE_NUM*len_factor)
                        {
                            GlobalVariable::trans_queue.pop_front();
                        }

                        delete cinfo;
                        maxCount++;
                        if(maxCount>=maxLimit)
                        {
                            break;
                        }
                    }
                }
                if (GlobalVariable::is_sync_done && dataBuffer.size()>0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }
            }

            //if (sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {

        }

        //vibrate
        try
        {
            maxCount = 0;
            //QQueue<QString> sqls;
            QMutexLocker m_lock(&GlobalVariable::vibratesglobalMutex);
            if(GlobalVariable::vibrates.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("vibrate");

                QList<QString> keyList = GlobalVariable::vibrates.keys();

                QList<QString>::const_iterator it,iit;

                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 4;
                QByteArray dataBuffer;

                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QMap<QString,QQueue<VibrateInfo*>> deviceData = GlobalVariable::vibrates[*it];
                    QList<QString> inkeyList = deviceData.keys();

                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        while(GlobalVariable::vibrates[*it][*iit].size() > 1)
                        {
                            VibrateInfo* vinfo = GlobalVariable::vibrates[*it][*iit].dequeue();

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(vinfo->rksj,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    QString sql = "insert into vibrate (vibrate_e,speed_e,dcode,mcode,pipe,rksj) values (";
                                    sql += QString::number(vinfo->vibrate_e) + "," + QString::number(vinfo->speed_e) + ",'" + vinfo->dcode + "','" + vinfo->mcode + "'," + vinfo->pipe + ",'" + vinfo->rksj + "')";
                                    sqls.enqueue(sql);
                                }
                            }

                            int len_factor = GlobalVariable::channelInfos.size() > 0 ? GlobalVariable::channelInfos.size() : 1;
                            if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM*len_factor)
                            {
                                QString cinfo_str = vinfo->toString() + ";";
                                dataBuffer.append(cinfo_str.toUtf8());
                            }

                            //qDebug() << "trans queue size: " << QString::number(GlobalVariable::trans_queue.size());
                            if(GlobalVariable::trans_queue.size()>=MAX_QUEUE_NUM*len_factor)
                            {
                                GlobalVariable::trans_queue.pop_front();
                            }

                            delete vinfo;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }

                if (GlobalVariable::is_sync_done && dataBuffer.size()>0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }

                //maxCount++;
                //if (maxCount > 2)
                //{
                //    break;
                //}
            }
            //if(sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {

        }

        //temperature
        try
        {
            maxCount = 0;
            //QQueue<QString> sqls;
            QMutexLocker m_lock(&GlobalVariable::temperaturesglobalMutex);
            if(GlobalVariable::temperatures.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("temperature");

                QList<QString> keyList = GlobalVariable::temperatures.keys();

                QList<QString>::const_iterator it,iit;

                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 7;
                QByteArray dataBuffer;

                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QMap<QString,QQueue<TemperatureInfo*>> deviceData = GlobalVariable::temperatures[*it];
                    QList<QString> inkeyList = deviceData.keys();
                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        while(GlobalVariable::temperatures[*it][*iit].size() > 1)
                        {
                            TemperatureInfo* tinfo = GlobalVariable::temperatures[*it][*iit].dequeue();

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(tinfo->rksj,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    QString sql = "insert into temperature (temp,dcode,mcode,pipe,rksj) values (";
                                    sql += QString::number(tinfo->temp) + ",'" + tinfo->dcode + "','" + tinfo->mcode + "'," + tinfo->pipe + ",'" + tinfo->rksj + "')";
                                    sqls.enqueue(sql);
                                }
                            }

                            int len_factor = GlobalVariable::channelInfos.size() > 0 ? GlobalVariable::channelInfos.size() : 1;
                            if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM*len_factor)
                            {
                                QString cinfo_str = tinfo->toString() + ";";
                                dataBuffer.append(cinfo_str.toUtf8());
                            }

                            //qDebug() << "trans queue size: " << QString::number(GlobalVariable::trans_queue.size());
                            if(GlobalVariable::trans_queue.size()>=MAX_QUEUE_NUM*len_factor)
                            {
                                GlobalVariable::trans_queue.pop_front();
                            }

                            delete tinfo;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }

                if(GlobalVariable::is_sync_done && dataBuffer.size() > 0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }

                //maxCount++;
                //if(maxCount>2)
                //{
                //    break;
                //}
            }
            //if(sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {

        }

        try
        {
            maxCount = 0;
            //QQueue<QString> sqls;

            int threshold = 0;
            /*
            if(GlobalVariable::isSaveData)
            {
                threshold = 0;
            }
            else
            {
                threshold = 1;
            }
            */

            while(GlobalVariable::conditions.size() > 0)
            {
                //save info

                QList<QString> keyList = GlobalVariable::conditions.keys();

                QList<QString>::const_iterator it;

                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QString deviceCode = *it;

                    while(GlobalVariable::conditions[deviceCode].size() > threshold)
                    {
                        ConditionInfo cinfo = GlobalVariable::conditions[deviceCode].head();

                        if(GlobalVariable::conditions[deviceCode].size() > 1)
                        {
                            cinfo = GlobalVariable::conditions[deviceCode].dequeue();
                        }

                        bool isRwave = false;
                        if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                        {
                            QDateTime dt1 = QDateTime::fromString(cinfo.rksj,GlobalVariable::dtFormat);
                            QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                            dt2 = dt2.addSecs(GlobalVariable::rwaveinterval);
                            int s_diff = dt1.secsTo(dt2);
                            if(s_diff > 0)
                            {
                                isRwave = true;
                            }
                        }

                        if(GlobalVariable::isSaveData || isRwave)
                        {
                            if(GlobalVariable::data_save_days > 0)
                            {
                                QString sql = "insert into condition (sample,rotate,interval,rksj,dcode,channel,wid) values (";
                                sql += QString::number(cinfo.sample) + "," + QString::number(cinfo.rotate,10,1) + "," + QString::number(cinfo.interval,10,1) + ",'" + cinfo.rksj + "','" + cinfo.dcode + "','1',0)";
                                sqls.enqueue(sql);
                            }
                        }

                        maxCount++;
                        if(maxCount>=maxLimit)
                        {
                            break;
                        }
                    }
                }

                maxCount++;
                if(maxCount>20)
                {
                    break;
                }
            }
            //if(sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}

        }
        catch(QException e)
        {

        }

        try
        {
            maxCount = 0;
            /*
            static int study_count = 0;
            if(!GlobalVariable::is_study)
            {
                study_count = 0;
            }
            */
            //QQueue<QString> sqls;

            QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
            if(GlobalVariable::vibrate_analyse.size() > 0)
            {
                //QSqlTableModel* model = mdb->modelNoHeader("electriccharge");

                QList<QString> keyList = GlobalVariable::vibrate_analyse.keys();
                QList<QString>::const_iterator it,iit;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QString it_v = *it;
                    QMap<QString,QQueue<VibrateAnyseDataLib*>> deviceData = GlobalVariable::vibrate_analyse[*it];
                    QList<QString> inkeyList = deviceData.keys();
                    //save info
                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        QString iit_v = *iit;
                        while(GlobalVariable::vibrate_analyse[it_v][iit_v].size() > 1 && !GlobalVariable::vibrate_analyse[it_v][iit_v].isEmpty())
                        {
                            VibrateAnyseDataLib* v_data = GlobalVariable::vibrate_analyse[it_v][iit_v].dequeue();

                            QString channel = *iit;
                            QString dcode = *it;
                            QString info_t = v_data->sample_time;

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                SVibTimeDmnFeat acc_time_feat = v_data->accTimeFeat;
                                float ampvalue = qIsFinite(acc_time_feat.AmpValue) ? acc_time_feat.AmpValue : 0.0;
                                float rmsvalue = qIsFinite(acc_time_feat.RMSValue) ? acc_time_feat.RMSValue : 0.0;
                                float pkpkvalue = qIsFinite(acc_time_feat.PkPkValue) ? acc_time_feat.PkPkValue : 0.0;
                                float waveindex = qIsFinite(acc_time_feat.waveIndex) ? acc_time_feat.waveIndex : 0.0;
                                float peakindex = qIsFinite(acc_time_feat.peakIndex) ? acc_time_feat.peakIndex : 0.0;
                                float pulsindex = qIsFinite(acc_time_feat.pulsIndex) ? acc_time_feat.pulsIndex : 0.0;
                                float kurtindex = qIsFinite(acc_time_feat.kurtIndex) ? acc_time_feat.kurtIndex : 0.0;

                                QString sql = "insert into acctimefeat_record (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe) values (";
                                sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                                     + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                                        + info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SVibFreqDmnFeat acc_freq_feat = v_data->accFreqFeat;
                                sql = "insert into accfreqfeat_record (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe) values (";
                                for(int n=0;n<11;n++)
                                {
                                    sql += QString::number(acc_freq_feat.vibMultFrq[n],10,4) + ",";
                                }
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SVibTimeDmnFeat spd_time_feat = v_data->spdTimeFeat;
                                ampvalue = qIsFinite(spd_time_feat.AmpValue) ? spd_time_feat.AmpValue : 0.0;
                                rmsvalue = qIsFinite(spd_time_feat.RMSValue) ? spd_time_feat.RMSValue : 0.0;
                                pkpkvalue = qIsFinite(spd_time_feat.PkPkValue) ? spd_time_feat.PkPkValue : 0.0;
                                waveindex = qIsFinite(spd_time_feat.waveIndex) ? spd_time_feat.waveIndex : 0.0;
                                peakindex = qIsFinite(spd_time_feat.peakIndex) ? spd_time_feat.peakIndex : 0.0;
                                pulsindex = qIsFinite(spd_time_feat.pulsIndex) ? spd_time_feat.pulsIndex : 0.0;
                                kurtindex = qIsFinite(spd_time_feat.kurtIndex) ? spd_time_feat.kurtIndex : 0.0;

                                sql = "insert into spdtimefeat_record (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe) values (";
                                sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                                     + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                                        + info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SVibFreqDmnFeat spd_freq_feat = v_data->spdFreqFeat;
                                sql = "insert into spdfreqfeat_record (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe) values (";
                                for(int n=0;n<11;n++)
                                {
                                    sql += QString::number(spd_freq_feat.vibMultFrq[n],10,4) + ",";
                                }
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SEnvFreqDmnFeat en_freq_feat = v_data->envFreqFeat;
                                sql = "insert into envfreqfeat_record (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,freq12,freq13,freq14,freq15,freq16,freq17,freq18,freq19,freq20,rksj,dcode,mcode,pipe) values (";
                                for(int n=0;n<20;n++)
                                {
                                    sql += QString::number(en_freq_feat.envMultFrq[n],10,4) + ",";
                                }
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                //qDebug() << "sql: " << sql;
                                sqls.enqueue(sql);
                            }
                            if(GlobalVariable::is_study)
                            {

                                QVector<int> tmps = GlobalVariable::findChannelTreeIdByDeviceAndChannel(dcode,channel);
                                QVector<int>::ConstIterator tcIt;
                                for(tcIt=tmps.constBegin();tcIt!=tmps.constEnd();tcIt++)
                                {
                                    QString mcode = "";
                                    int tmp = *tcIt;
                                    if(tmp > 0)
                                    {
                                        tmp = GlobalVariable::findMotorTreeParentId(tmp);
                                        if(tmp > 0)
                                        {
                                            mcode = GlobalVariable::findMotorCodeById(tmp);
                                        }
                                    }

                                    int mctId=0;
                                    if(mcode.length()>0)
                                    {
                                        if(GlobalVariable::motorConditionTables.contains(mcode) && GlobalVariable::motorCondition.contains(mcode))
                                        {
                                            float cur_rotate = GlobalVariable::motorCondition[mcode].rotate;
                                            int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                            int rotate_threshold = 250;
                                            if(rotate_s > 0)
                                                rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;
                                            QVector<MotorConditionTable>::ConstIterator mctIt;
                                            QVector<MotorConditionTable> mcs = GlobalVariable::motorConditionTables[mcode];
                                            for(mctIt=mcs.constBegin();mctIt!=mcs.constEnd();mctIt++)
                                            {
                                                MotorConditionTable mct = *mctIt;
                                                if(qAbs(mct.rotate - cur_rotate) < rotate_threshold)
                                                {
                                                    mctId = mct.id;
                                                }
                                            }
                                        }
                                        if(mctId > 0)
                                        {
                                            SVibTimeDmnFeat acc_time_feat = v_data->accTimeFeat;
                                            float ampvalue = qIsFinite(acc_time_feat.AmpValue) ? acc_time_feat.AmpValue : 0.0;
                                            float rmsvalue = qIsFinite(acc_time_feat.RMSValue) ? acc_time_feat.RMSValue : 0.0;
                                            float pkpkvalue = qIsFinite(acc_time_feat.PkPkValue) ? acc_time_feat.PkPkValue : 0.0;
                                            float waveindex = qIsFinite(acc_time_feat.waveIndex) ? acc_time_feat.waveIndex : 0.0;
                                            float peakindex = qIsFinite(acc_time_feat.peakIndex) ? acc_time_feat.peakIndex : 0.0;
                                            float pulsindex = qIsFinite(acc_time_feat.pulsIndex) ? acc_time_feat.pulsIndex : 0.0;
                                            float kurtindex = qIsFinite(acc_time_feat.kurtIndex) ? acc_time_feat.kurtIndex : 0.0;

                                            QString sql = "insert into acctimefeat (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe,mcid) values (";
                                            sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                                                 + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                                                    + info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);
                                            //qDebug() << sql;

                                            SVibFreqDmnFeat acc_freq_feat = v_data->accFreqFeat;
                                            sql = "insert into accfreqfeat (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                                            for(int n=0;n<11;n++)
                                            {
                                                sql += QString::number(acc_freq_feat.vibMultFrq[n],10,4) + ",";
                                            }
                                            sql += "'"+ info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SVibTimeDmnFeat spd_time_feat = v_data->spdTimeFeat;
                                            ampvalue = qIsFinite(spd_time_feat.AmpValue) ? spd_time_feat.AmpValue : 0.0;
                                            rmsvalue = qIsFinite(spd_time_feat.RMSValue) ? spd_time_feat.RMSValue : 0.0;
                                            pkpkvalue = qIsFinite(spd_time_feat.PkPkValue) ? spd_time_feat.PkPkValue : 0.0;
                                            waveindex = qIsFinite(spd_time_feat.waveIndex) ? spd_time_feat.waveIndex : 0.0;
                                            peakindex = qIsFinite(spd_time_feat.peakIndex) ? spd_time_feat.peakIndex : 0.0;
                                            pulsindex = qIsFinite(spd_time_feat.pulsIndex) ? spd_time_feat.pulsIndex : 0.0;
                                            kurtindex = qIsFinite(spd_time_feat.kurtIndex) ? spd_time_feat.kurtIndex : 0.0;

                                            sql = "insert into spdtimefeat (amp,rms,pkpk,waveindex,peakindex,pulsindex,kurtindex,rksj,dcode,mcode,pipe,mcid) values (";
                                            sql += QString::number(ampvalue,10,4) + "," + QString::number(rmsvalue,10,4) + "," + QString::number(pkpkvalue,10,4) + "," \
                                                 + QString::number(waveindex,10,4) + "," + QString::number(peakindex,10,4) + "," + QString::number(pulsindex,10,4) + "," + QString::number(kurtindex,10,4) + ",'" \
                                                    + info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SVibFreqDmnFeat spd_freq_feat = v_data->spdFreqFeat;
                                            sql = "insert into spdfreqfeat (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                                            for(int n=0;n<11;n++)
                                            {
                                                sql += QString::number(spd_freq_feat.vibMultFrq[n],10,4) + ",";
                                            }
                                            sql += "'"+ info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SEnvFreqDmnFeat en_freq_feat = v_data->envFreqFeat;
                                            sql = "insert into envfreqfeat (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,freq12,freq13,freq14,freq15,freq16,freq17,freq18,freq19,freq20,rksj,dcode,mcode,pipe,mcid) values (";
                                            for(int n=0;n<20;n++)
                                            {
                                                sql += QString::number(en_freq_feat.envMultFrq[n],10,4) + ",";
                                            }
                                            sql += "'"+ info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            //envlop freq feat

                                            /*
                                            study_count++;

                                            if(study_count>=GlobalVariable::study_num)
                                            {
                                                GlobalVariable::is_study = false;
                                            }
                                            */
                                        }
                                    }
                                }
                            }
                            delete v_data;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
            }

            //if (sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {
            qDebug()<< e.what();
        }

        try
        {
            maxCount = 0;
            //QQueue<QString> sqls;

            QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
            if(GlobalVariable::electric_analyse.size() > 0)
            {
                //QSqlTableModel* model = mdb->modelNoHeader("electriccharge");

                QList<QString> keyList = GlobalVariable::electric_analyse.keys();
                QList<QString>::const_iterator it,iit;

                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QMap<QString,QQueue<ElectricAnyseDataLib*>> deviceData = GlobalVariable::electric_analyse[*it];
                    QList<QString> inkeyList = deviceData.keys();
                    //save info
                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        while(GlobalVariable::electric_analyse[*it][*iit].size() > 1)
                        {
                            ElectricAnyseDataLib* e_data = GlobalVariable::electric_analyse[*it][*iit].dequeue();

                            QString info_t = e_data->sample_time;

                            QString channel = *iit;
                            QString dcode = *it;

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);;
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                SElcTimeDmnFeat vol_time_feat = e_data->volTimeFeat;
                                QString sql = "insert into voltimefeat_record (amp,rms,pkpk,rksj,dcode,mcode,pipe) values (";
                                sql += QString::number(vol_time_feat.AmpValue,10,4) + "," + QString::number(vol_time_feat.RMSValue,10,4) + "," + QString::number(vol_time_feat.PkPkValue,10,4) + ",'" \
                                        + info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SElcFreqDmnFeat vol_freq_feat = e_data->volFreqFeat;
                                sql = "insert into volfreqfeat_record (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe) values (";
                                for(int n=0;n<11;n++)
                                {
                                    sql += QString::number(vol_freq_feat.elcMultFrq[n],10,4) + ",";
                                }
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SElcTimeDmnFeat cur_time_feat = e_data->curTimeFeat;
                                sql = "insert into curtimefeat_record (amp,rms,pkpk,rksj,dcode,mcode,pipe) values (";
                                sql += QString::number(cur_time_feat.AmpValue,10,4) + "," + QString::number(cur_time_feat.RMSValue,10,4) + "," + QString::number(cur_time_feat.PkPkValue,10,4) + ",'" \
                                        + info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                SElcFreqDmnFeat cur_freq_feat = e_data->curFreqFeat;
                                sql = "insert into curfreqfeat_record (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe) values (";
                                for(int n=0;n<11;n++)
                                {
                                    sql += QString::number(cur_freq_feat.elcMultFrq[n],10,4) + ",";
                                }
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);

                                sql = "insert into electricanalysedata (upsequence,unsequence,uzsequence,ipsequence,insequence,izsequence,actpower,reactpower,apppower,factor,rksj,dcode,mcode,pipe) values (";
                                sql += QString::number(e_data->vVolComp[0],10,4) + "," + QString::number(e_data->vVolComp[1],10,4) + "," + QString::number(e_data->vVolComp[2],10,4) + ",";
                                sql += QString::number(e_data->vCurComp[0],10,4) + "," + QString::number(e_data->vCurComp[1],10,4) + "," + QString::number(e_data->vCurComp[2],10,4) + ",";
                                sql += QString::number(e_data->powerAnsysFeat.actPower,10,4) + "," + QString::number(e_data->powerAnsysFeat.reactPower,10,4) + "," + QString::number(e_data->powerAnsysFeat.appPower,10,4) + "," + QString::number(e_data->powerAnsysFeat.cosf,10,4) + ",";
                                sql += "'"+ info_t + "','" + dcode + "','0'," + channel + ")";
                                sqls.enqueue(sql);
                            }
                            if(GlobalVariable::is_study)
                            {
                                QVector<int> tmps = GlobalVariable::findChannelTreeIdByDeviceAndChannel(dcode,channel);
                                QVector<int>::ConstIterator tcIt;
                                for(tcIt=tmps.constBegin();tcIt!=tmps.constEnd();tcIt++)
                                {
                                    QString mcode = "";
                                    int tmp = *tcIt;
                                    if(tmp > 0)
                                    {
                                        tmp = GlobalVariable::findMotorTreeParentId(tmp);
                                        if(tmp > 0)
                                        {
                                            mcode = GlobalVariable::findMotorCodeById(tmp);
                                        }
                                    }

                                    int mctId=0;
                                    if(mcode.length()>0)
                                    {
                                        if(GlobalVariable::motorConditionTables.contains(mcode) && GlobalVariable::motorCondition.contains(mcode))
                                        {
                                            float cur_rotate = GlobalVariable::motorCondition[mcode].rotate;
                                            int rotate_s = GlobalVariable::findMotorRotateByCode(mcode);
                                            int rotate_threshold = 250;
                                            if(rotate_s > 0)
                                                rotate_threshold = rotate_s * GlobalVariable::diagnose_rotate_diff;
                                            QVector<MotorConditionTable>::ConstIterator mctIt;
                                            QVector<MotorConditionTable> mcs = GlobalVariable::motorConditionTables[mcode];
                                            for(mctIt=mcs.constBegin();mctIt!=mcs.constEnd();mctIt++)
                                            {
                                                MotorConditionTable mct = *mctIt;
                                                if(qAbs(mct.rotate - cur_rotate) < rotate_threshold)
                                                {
                                                    mctId = mct.id;
                                                }
                                            }
                                        }
                                        if(mctId > 0)
                                        {
                                            SElcTimeDmnFeat vol_time_feat = e_data->volTimeFeat;
                                            QString sql = "insert into voltimefeat (amp,rms,pkpk,rksj,dcode,mcode,pipe,mcid) values (";
                                            sql += QString::number(vol_time_feat.AmpValue,10,4) + "," + QString::number(vol_time_feat.RMSValue,10,4) + "," + QString::number(vol_time_feat.PkPkValue,10,4) + ",'" \
                                                    + info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SElcFreqDmnFeat vol_freq_feat = e_data->volFreqFeat;
                                            sql = "insert into volfreqfeat (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                                            for(int n=0;n<11;n++)
                                            {
                                                sql += QString::number(vol_freq_feat.elcMultFrq[n],10,4) + ",";
                                            }
                                            sql += "'"+ info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SElcTimeDmnFeat cur_time_feat = e_data->curTimeFeat;
                                            sql = "insert into curtimefeat (amp,rms,pkpk,rksj,dcode,mcode,pipe,mcid) values (";
                                            sql += QString::number(cur_time_feat.AmpValue,10,4) + "," + QString::number(cur_time_feat.RMSValue,10,4) + "," + QString::number(cur_time_feat.PkPkValue,10,4) + ",'" \
                                                    + info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);

                                            SElcFreqDmnFeat cur_freq_feat = e_data->curFreqFeat;
                                            sql = "insert into curfreqfeat (freq1,freq2,freq3,freq4,freq5,freq6,freq7,freq8,freq9,freq10,freq11,rksj,dcode,mcode,pipe,mcid) values (";
                                            for(int n=0;n<11;n++)
                                            {
                                                sql += QString::number(cur_freq_feat.elcMultFrq[n],10,4) + ",";
                                            }
                                            sql += "'"+ info_t + "','" + dcode + "','" + mcode + "'," + channel + "," + QString::number(mctId) + ")";
                                            sqls.enqueue(sql);
                                        }
                                    }
                                }
                            }
                            delete e_data;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
            }

            //if (sqls.size()>0)
            //{
            //    mdb->execSql(sqls);
            //}
        }
        catch(QException e)
        {
            qDebug()<< e.what();
        }

        //electric global park
        try
        {
            QMutexLocker m_lock(&GlobalVariable::elcglobalMutex);
            if(GlobalVariable::elcglobaldatas.size() > 0)
            {
                //save info
                QList<QString> keyList = GlobalVariable::elcglobaldatas.keys();

                QList<QString>::const_iterator it;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QString mcode = *it;

                    QQueue<ElectricGlobal*> parks = GlobalVariable::elcglobaldatas[mcode];
                    QQueue<ElectricGlobal*>::ConstIterator iit;

                    for(iit=parks.constBegin();iit!=parks.constEnd();iit++)
                    {
                        while(GlobalVariable::elcglobaldatas[mcode].size() > 1)
                        {
                            ElectricGlobal* finfo = GlobalVariable::elcglobaldatas[mcode].dequeue();

                            QString info_t = finfo->sample_time;
                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(info_t,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    QString vol_x_str = "";
                                    std::vector<double>::iterator pit;
                                    for(pit=finfo->vol_park_x.begin();pit!=finfo->vol_park_x.end();pit++)
                                    {
                                        vol_x_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(vol_x_str.length()>0)
                                    {
                                        vol_x_str = vol_x_str.left(vol_x_str.length()-1);
                                    }

                                    QString vol_y_str = "";
                                    for(pit=finfo->vol_park_y.begin();pit!=finfo->vol_park_y.end();pit++)
                                    {
                                        vol_y_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(vol_y_str.length()>0)
                                    {
                                        vol_y_str = vol_y_str.left(vol_y_str.length()-1);
                                    }

                                    QString vol_freq_str = "";
                                    for(pit=finfo->vol_park_freq.begin();pit!=finfo->vol_park_freq.end();pit++)
                                    {
                                        vol_freq_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(vol_freq_str.length()>0)
                                    {
                                        vol_freq_str = vol_freq_str.left(vol_freq_str.length()-1);
                                    }

                                    QString cur_x_str = "";
                                    for(pit=finfo->cur_park_x.begin();pit!=finfo->cur_park_x.end();pit++)
                                    {
                                        cur_x_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(cur_x_str.length()>0)
                                    {
                                        cur_x_str = cur_x_str.left(cur_x_str.length()-1);
                                    }

                                    QString cur_y_str = "";
                                    for(pit=finfo->cur_park_y.begin();pit!=finfo->cur_park_y.end();pit++)
                                    {
                                        cur_y_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(cur_y_str.length()>0)
                                    {
                                        cur_y_str = cur_y_str.left(cur_y_str.length()-1);
                                    }

                                    QString cur_freq_str = "";
                                    for(pit=finfo->cur_park_freq.begin();pit!=finfo->cur_park_freq.end();pit++)
                                    {
                                        cur_freq_str += QString::number(*pit,10,4) + ",";
                                    }
                                    if(cur_freq_str.length()>0)
                                    {
                                        cur_freq_str = cur_freq_str.left(cur_freq_str.length()-1);
                                    }

                                    QString sql = "insert into parktransform (vol_x,vol_y,vol_freq,cur_x,cur_y,cur_freq,rksj,dcode,mcode) values (";
                                    sql += "'" + vol_x_str + "','" + vol_y_str + "','" + vol_freq_str + "','" + cur_x_str + "','" + cur_y_str + "','" + cur_freq_str + "','" + info_t + "','" + finfo->dcode + "','" + mcode + "')";
                                    sqls.enqueue(sql);
                                }
                            }

                            delete finfo;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
        catch(QException e)
        {

        }

        try
        {
            if (sqls.size()>0)
            {
                mdb->execSql(sqls);
            }
        }
        catch(QException e)
        {

        }

        //charge freq
        try
        {
            QMutexLocker m_lock(&GlobalVariable::freqsglobalMutex);
            if(GlobalVariable::freqs.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("electricchargewavefreq");

                QList<QString> keyList = GlobalVariable::freqs.keys();

                /*
                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 2;
                QByteArray dataBuffer;
                */

                QList<QString>::const_iterator it,iit;
                QMap<QString,QMap<QString,QVariantList>> binds;
                QMap<QString,QVariantList> efreqs,vfreqs;
                binds["E"] = efreqs;
                binds["V"] = vfreqs;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QString deviceCode = *it;
                    QString deviceType = GlobalVariable::findDeviceTypeByCode(deviceCode);

                    QMap<QString,QQueue<FreqInfo*>> deviceFreq = GlobalVariable::freqs[deviceCode];
                    QList<QString> inkeyList = deviceFreq.keys();

                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        //int tmplen = GlobalVariable::freqs[deviceCode][*iit].size();
                        QString channel_code = *iit;
                        while(GlobalVariable::freqs.contains(deviceCode) && GlobalVariable::freqs[deviceCode].contains(channel_code) && GlobalVariable::freqs[deviceCode][channel_code].size() > 2)
                        {
                            FreqInfo* finfo = GlobalVariable::freqs[deviceCode][channel_code].dequeue();

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(finfo->rksj,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    if(binds[deviceType].contains("sample_freq"))
                                    {
                                        binds[deviceType]["sample_freq"].append(finfo->sample_freq);
                                    }
                                    else
                                    {
                                        QVariantList freqs;
                                        freqs.append(finfo->sample_freq);
                                        binds[deviceType].insert("sample_freq",freqs);
                                    }
                                    if(binds[deviceType].contains("dcode"))
                                    {
                                        binds[deviceType]["dcode"].append(finfo->dcode);
                                    }
                                    else
                                    {
                                        QVariantList dcodes;
                                        dcodes.append(finfo->dcode);
                                        binds[deviceType].insert("dcode",dcodes);
                                    }
                                    if(binds[deviceType].contains("mcode"))
                                    {
                                        binds[deviceType]["mcode"].append(finfo->mcode);
                                    }
                                    else
                                    {
                                        QVariantList mcodes;
                                        mcodes.append(finfo->mcode);
                                        binds[deviceType].insert("mcode",mcodes);
                                    }
                                    if(binds[deviceType].contains("pipe"))
                                    {
                                        binds[deviceType]["pipe"].append(finfo->pipe);
                                    }
                                    else
                                    {
                                        QVariantList pipes;
                                        pipes.append(finfo->pipe);
                                        binds[deviceType].insert("pipe",pipes);
                                    }
                                    if(binds[deviceType].contains("rksj"))
                                    {
                                        binds[deviceType]["rksj"].append(finfo->rksj);
                                    }
                                    else
                                    {
                                        QVariantList rksjs;
                                        rksjs.append(finfo->rksj);
                                        binds[deviceType].insert("rksj",rksjs);
                                    }
                                    if(binds[deviceType].contains("stype"))
                                    {
                                        binds[deviceType]["stype"].append(finfo->stype);
                                    }
                                    else
                                    {
                                        QVariantList stypes;
                                        stypes.append(finfo->stype);
                                        binds[deviceType].insert("stype",stypes);
                                    }
                                }
                            }

                            /*
                            if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM)
                            {
                                QString cinfo_str = finfo->toString() + ";";
                                dataBuffer.append(cinfo_str.toUtf8());
                            }
                            */

                            delete finfo;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
                /*
                if(dataBuffer.size() > 0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }
                */

                if(binds["E"].size()>0)
                {
                    QString sql = "insert into electricchargewavefreq (sample_freq,dcode,mcode,pipe,rksj,stype) values (:sample_freq,:dcode,:mcode,:pipe,:rksj,:stype)";
                    mdb->execSql(sql,binds["E"]);
                }
                if(binds["V"].size()>0)
                {
                    QString sql = "insert into vibratewavefreq (sample_freq,dcode,mcode,pipe,rksj,stype) values (:sample_freq,:dcode,:mcode,:pipe,:rksj,:stype)";
                    mdb->execSql(sql,binds["V"]);
                }
            }
        }
        catch(QException e)
        {

        }

        //charge wave
        try
        {
            QMutexLocker m_lock(&GlobalVariable::wavesglobalMutex);
            if(GlobalVariable::waves.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("electricchargewavefreq");

                QList<QString> keyList = GlobalVariable::waves.keys();

                /*
                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 2;
                QByteArray dataBuffer;
                */

                QList<QString>::const_iterator it,iit;
                QMap<QString,QMap<QString,QVariantList>> binds;
                QMap<QString,QVariantList> efreqs,vfreqs;
                binds["E"] = efreqs;
                binds["V"] = vfreqs;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    QString deviceCode = *it;
                    QString deviceType = GlobalVariable::findDeviceTypeByCode(deviceCode).trimmed();
                    //qDebug() << "deviceCode : " << deviceCode << "deviceType : " << deviceType;

                    QMap<QString,QQueue<WaveInfo*>> deviceWave = GlobalVariable::waves[deviceCode];
                    QList<QString> inkeyList = deviceWave.keys();

                    for(iit=inkeyList.constBegin();iit!=inkeyList.constEnd();iit++)
                    {
                        QString channel_code = *iit;
                        while(GlobalVariable::waves[deviceCode][channel_code].size() > 2)
                        {
                            WaveInfo* winfo = GlobalVariable::waves[deviceCode][channel_code].dequeue();

                            bool isRwave = false;
                            if(GlobalVariable::rwaveinterval > 0 && !GlobalVariable::isSaveData)
                            {
                                QDateTime dt1 = QDateTime::fromString(winfo->rksj,GlobalVariable::dtFormat);
                                QDateTime dt2 = QDateTime::fromString(GlobalVariable::rwavetime,GlobalVariable::dtFormat);
                                int s_diff = dt2.secsTo(dt1);
                                if(s_diff > 0 && s_diff < GlobalVariable::rwaveinterval)
                                {
                                    isRwave = true;
                                }
                            }

                            if(GlobalVariable::isSaveData || isRwave)
                            {
                                if(GlobalVariable::data_save_days > 0)
                                {
                                    if(binds[deviceType].contains("sample_data"))
                                    {
                                        binds[deviceType]["sample_data"].append(winfo->sample_data);
                                    }
                                    else
                                    {
                                        QVariantList freqs;
                                        freqs.append(winfo->sample_data);
                                        binds[deviceType].insert("sample_data",freqs);
                                    }
                                    if(binds[deviceType].contains("dcode"))
                                    {
                                        binds[deviceType]["dcode"].append(winfo->dcode);
                                    }
                                    else
                                    {
                                        QVariantList dcodes;
                                        dcodes.append(winfo->dcode);
                                        binds[deviceType].insert("dcode",dcodes);
                                    }
                                    if(binds[deviceType].contains("mcode"))
                                    {
                                        binds[deviceType]["mcode"].append(winfo->mcode);
                                    }
                                    else
                                    {
                                        QVariantList mcodes;
                                        mcodes.append(winfo->mcode);
                                        binds[deviceType].insert("mcode",mcodes);
                                    }
                                    if(binds[deviceType].contains("pipe"))
                                    {
                                        binds[deviceType]["pipe"].append(winfo->pipe);
                                    }
                                    else
                                    {
                                        QVariantList pipes;
                                        pipes.append(winfo->pipe);
                                        binds[deviceType].insert("pipe",pipes);
                                    }
                                    if(binds[deviceType].contains("rksj"))
                                    {
                                        binds[deviceType]["rksj"].append(winfo->rksj);
                                    }
                                    else
                                    {
                                        QVariantList rksjs;
                                        rksjs.append(winfo->rksj);
                                        binds[deviceType].insert("rksj",rksjs);
                                    }
                                    if(binds[deviceType].contains("stype"))
                                    {
                                        binds[deviceType]["stype"].append(winfo->stype);
                                    }
                                    else
                                    {
                                        QVariantList stypes;
                                        stypes.append(winfo->stype);
                                        binds[deviceType].insert("stype",stypes);
                                    }
                                    if(binds[deviceType].contains("wid"))
                                    {
                                        binds[deviceType]["wid"].append(winfo->is_Continuity);
                                    }
                                    else
                                    {
                                        QVariantList stypes;
                                        stypes.append(winfo->is_Continuity);
                                        binds[deviceType].insert("wid",stypes);
                                    }
                                }
                            }

                            /*
                            if(GlobalVariable::server_enable && GlobalVariable::trans_queue.size()<MAX_QUEUE_NUM)
                            {
                                QString cinfo_str = winfo->toString() + ";";
                                dataBuffer.append(cinfo_str.toUtf8());
                            }
                            */

                            delete winfo;

                            maxCount++;
                            if(maxCount>=maxLimit)
                            {
                                break;
                            }
                        }
                    }
                }
                /*
                if(dataBuffer.size() > 0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    GlobalVariable::trans_queue.enqueue(sendBuffer);
                }
                */

                //qDebug() << "E size: " << binds["E"].size();
                //qDebug() << "V size: " << binds["V"].size();
                if(binds["E"].size()>0)
                {
                    QString sql = "insert into electricchargewave (sample_data,dcode,mcode,pipe,rksj,stype,wid) values (:sample_data,:dcode,:mcode,:pipe,:rksj,:stype,:wid)";
                    mdb->execSql(sql,binds["E"]);
                }
                //qDebug() << "V size: " << binds["V"].size();
                if(binds["V"].size()>0)
                {
                    QString sql = "insert into vibratewave (sample_data,dcode,mcode,pipe,rksj,stype,wid) values (:sample_data,:dcode,:mcode,:pipe,:rksj,:stype,:wid)";
                    mdb->execSql(sql,binds["V"]);
                }
            }
        }
        catch(QException e)
        {

        }

        //end = QDateTime::currentDateTime();

        //qint64 time_diff = start.secsTo(end);
        //qDebug() << "save data cost time: " << QString::number(time_diff);

        {
            msleep(500);
        }
        //msleep(500);
    }
}
