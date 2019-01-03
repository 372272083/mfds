#include "savesamplethread.h"

#include <QException>
#include <QSqlTableModel>
#include <QList>

#include "mainwindow.h"
#include "sqlitedb.h"

#include "chargeinfo.h"
#include "vibrateinfo.h"
#include "temperatureinfo.h"

#include "freqinfo.h"
#include "waveinfo.h"

#include <QDebug>

SaveSampleThread::SaveSampleThread(SqliteDB *db):mdb(db)
{

}

void SaveSampleThread::run()
{
    int row = 0;
    int maxCount = 0;
    while(true)
    {
        try
        {
            maxCount = 0;
            QQueue<QString> sqls;

            while(MainWindow::charges.size() > 0)
            {
                //QSqlTableModel* model = mdb->modelNoHeader("electriccharge");

                QList<QString> keyList = MainWindow::charges.keys();

                QList<QString>::const_iterator it;
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
                    //save info
                    while(MainWindow::charges[*it].size() > 1)
                    {
                        ChargeInfo* cinfo = MainWindow::charges[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            QString sql = "insert into electriccharge (u,i,f,factor,p,q,s,others,pqs,dcode,mcode,pipe,rksj) values (";
                            sql += QString::number(cinfo->u) + "," + QString::number(cinfo->i) + "," + QString::number(cinfo->f) + "," + QString::number(cinfo->factor) + "," + QString::number(cinfo->p) + "," + QString::number(cinfo->q) + "," + QString::number(cinfo->s);
                            sql += ",'" + cinfo->others + "','" + cinfo->pqs + "','" + cinfo->dcode + "','" + cinfo->mcode + "'," + cinfo->pipe + ",'" + cinfo->rksj + "')";
                            sqls.enqueue(sql);
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = cinfo->toString() + ";";
                            //qDebug() << cinfo_str;
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete cinfo;
                    }
                }
                if (dataBuffer.size()>0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    MainWindow::trans_queue.enqueue(sendBuffer);
                }
                maxCount++;
                if (maxCount > 2)
                {
                    break;
                }
            }

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
            if(MainWindow::freqs.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("electricchargewavefreq");

                QList<QString> keyList = MainWindow::freqs.keys();

                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 2;
                QByteArray dataBuffer;

                QList<QString>::const_iterator it;
                QMap<QString,QVariantList> binds;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    while(MainWindow::freqs[*it].size() > 2)
                    {
                        FreqInfo* finfo = MainWindow::freqs[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            if(binds.contains("sample_freq"))
                            {
                                binds["sample_freq"].append(finfo->sample_freq);
                            }
                            else
                            {
                                QVariantList freqs;
                                freqs.append(finfo->sample_freq);
                                binds.insert("sample_freq",freqs);
                            }
                            if(binds.contains("dcode"))
                            {
                                binds["dcode"].append(finfo->dcode);
                            }
                            else
                            {
                                QVariantList dcodes;
                                dcodes.append(finfo->dcode);
                                binds.insert("dcode",dcodes);
                            }
                            if(binds.contains("mcode"))
                            {
                                binds["mcode"].append(finfo->mcode);
                            }
                            else
                            {
                                QVariantList mcodes;
                                mcodes.append(finfo->mcode);
                                binds.insert("mcode",mcodes);
                            }
                            if(binds.contains("pipe"))
                            {
                                binds["pipe"].append(finfo->pipe);
                            }
                            else
                            {
                                QVariantList pipes;
                                pipes.append(finfo->pipe);
                                binds.insert("pipe",pipes);
                            }
                            if(binds.contains("rksj"))
                            {
                                binds["rksj"].append(finfo->rksj);
                            }
                            else
                            {
                                QVariantList rksjs;
                                rksjs.append(finfo->rksj);
                                binds.insert("rksj",rksjs);
                            }
                            if(binds.contains("stype"))
                            {
                                binds["stype"].append(finfo->stype);
                            }
                            else
                            {
                                QVariantList stypes;
                                stypes.append(finfo->stype);
                                binds.insert("stype",stypes);
                            }
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = finfo->toString() + ";";
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete finfo;
                    }
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

                if(binds.size()>0)
                {
                    QString sql = "insert into electricchargewavefreq (sample_freq,dcode,mcode,pipe,rksj,stype) values (:sample_freq,:dcode,:mcode,:pipe,:rksj,:stype)";
                    mdb->execSql(sql,binds);
                }
                //delete model;
            }
        }
        catch(QException e)
        {

        }

        //charge wave
        try
        {
            if(MainWindow::waves.size() > 0)
            {

                //save info
                QSqlTableModel* model = mdb->modelNoHeader("electricchargewave");

                QList<QString> keyList = MainWindow::waves.keys();

                QList<QString>::const_iterator it;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    while(MainWindow::waves[*it].size() > 0)
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

                        WaveInfo* winfo = MainWindow::waves[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            model->insertRow(row);
                            model->setData(model->index(row,1),winfo->sample_data);
                            model->setData(model->index(row,2),winfo->dcode);
                            model->setData(model->index(row,3),winfo->mcode);
                            model->setData(model->index(row,4),winfo->pipe);
                            model->setData(model->index(row,5),winfo->rksj);
                            model->setData(model->index(row,6),winfo->stype);
                            model->submitAll();
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue_pri.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = winfo->toString();
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete winfo;

                        if(dataBuffer.size() > 0)
                        {
                            //int originLen = dataBuffer.length();
                            QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                            int compress_data_len = compress_dataBuffer.size();
                            sendBuffer[3] = compress_data_len / 0xFF;
                            sendBuffer[4] = compress_data_len % 0xFF;
                            sendBuffer.append(compress_dataBuffer);
                            MainWindow::trans_queue_pri.enqueue(sendBuffer);
                        }
                    }
                }

                delete model;
            }
        }
        catch(QException e)
        {

        }

        //vibrate
        try
        {
            maxCount = 0;
            QQueue<QString> sqls;
            while(MainWindow::vibrates.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("vibrate");

                QList<QString> keyList = MainWindow::vibrates.keys();

                QList<QString>::const_iterator it;

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
                    while(MainWindow::vibrates[*it].size() > 1)
                    {
                        VibrateInfo* vinfo = MainWindow::vibrates[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            QString sql = "insert into vibrate (vibrate_e,speed_e,dcode,mcode,pipe,rksj) values (";
                            sql += QString::number(vinfo->vibrate_e) + "," + QString::number(vinfo->speed_e) + ",'" + vinfo->dcode + "','" + vinfo->mcode + "'," + vinfo->pipe + ",'" + vinfo->rksj + "')";
                            sqls.enqueue(sql);
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = vinfo->toString() + ";";
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete vinfo;
                    }
                }

                if (dataBuffer.size()>0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    MainWindow::trans_queue.enqueue(sendBuffer);
                }

                maxCount++;
                if (maxCount > 2)
                {
                    break;
                }
            }
            if(sqls.size()>0)
            {
                mdb->execSql(sqls);
            }
        }
        catch(QException e)
        {

        }

        //vibrate freq
        try
        {
            if(MainWindow::freqs_v.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("vibratewavefreq");

                QList<QString> keyList = MainWindow::freqs_v.keys();

                QList<QString>::const_iterator it;

                QByteArray sendBuffer;
                sendBuffer.resize(6);
                sendBuffer[0] = 0;
                sendBuffer[1] = 0xFF;
                sendBuffer[2] = 0xFF;
                sendBuffer[3] = 0;
                sendBuffer[4] = 0;
                sendBuffer[5] = 5;
                QByteArray dataBuffer;

                QMap<QString,QVariantList> binds;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    while(MainWindow::freqs_v[*it].size() > 1)
                    {
                        FreqInfo* finfo = MainWindow::freqs_v[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            if(binds.contains("sample_freq"))
                            {
                                binds["sample_freq"].append(finfo->sample_freq);
                            }
                            else
                            {
                                QVariantList freqs;
                                freqs.append(finfo->sample_freq);
                                binds.insert("sample_freq",freqs);
                            }
                            if(binds.contains("dcode"))
                            {
                                binds["dcode"].append(finfo->dcode);
                            }
                            else
                            {
                                QVariantList dcodes;
                                dcodes.append(finfo->dcode);
                                binds.insert("dcode",dcodes);
                            }
                            if(binds.contains("mcode"))
                            {
                                binds["mcode"].append(finfo->mcode);
                            }
                            else
                            {
                                QVariantList mcodes;
                                mcodes.append(finfo->mcode);
                                binds.insert("mcode",mcodes);
                            }
                            if(binds.contains("pipe"))
                            {
                                binds["pipe"].append(finfo->pipe);
                            }
                            else
                            {
                                QVariantList pipes;
                                pipes.append(finfo->pipe);
                                binds.insert("pipe",pipes);
                            }
                            if(binds.contains("rksj"))
                            {
                                binds["rksj"].append(finfo->rksj);
                            }
                            else
                            {
                                QVariantList rksjs;
                                rksjs.append(finfo->rksj);
                                binds.insert("rksj",rksjs);
                            }
                            if(binds.contains("stype"))
                            {
                                binds["stype"].append(finfo->stype);
                            }
                            else
                            {
                                QVariantList stypes;
                                stypes.append(finfo->stype);
                                binds.insert("stype",stypes);
                            }
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = finfo->toString() + ";";
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete finfo;
                    }
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

                if(binds.size()>0)
                {
                    QString sql = "insert into vibratewavefreq (sample_freq,dcode,mcode,pipe,rksj,stype) values (:sample_freq,:dcode,:mcode,:pipe,:rksj,:stype)";
                    mdb->execSql(sql,binds);
                }
            }
        }
        catch(QException e)
        {

        }

        //vibrate wave
        try
        {
            if(MainWindow::waves_v.size() > 0)
            {
                //save info
                QSqlTableModel* model = mdb->modelNoHeader("vibratewave");

                QList<QString> keyList = MainWindow::waves_v.keys();

                QList<QString>::const_iterator it;
                for(it=keyList.constBegin();it!=keyList.constEnd();it++)
                {
                    while(MainWindow::waves_v[*it].size() > 0)
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

                        WaveInfo* winfo = MainWindow::waves_v[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            model->insertRow(row);
                            model->setData(model->index(row,1),winfo->sample_data);
                            model->setData(model->index(row,2),winfo->dcode);
                            model->setData(model->index(row,3),winfo->mcode);
                            model->setData(model->index(row,4),winfo->pipe);
                            model->setData(model->index(row,5),winfo->rksj);
                            model->setData(model->index(row,6),winfo->stype);
                            model->submitAll();
                        }

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = winfo->toString();
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete winfo;

                        if(dataBuffer.size() > 0)
                        {
                            QByteArray compress_dataBuffer = qCompress(dataBuffer,9);
                            int compress_data_len = compress_dataBuffer.size();
                            sendBuffer[3] = compress_data_len / 0xFF;
                            sendBuffer[4] = compress_data_len % 0xFF;
                            sendBuffer.append(compress_dataBuffer);
                            MainWindow::trans_queue_pri.enqueue(sendBuffer);
                        }
                    }
                }
                delete model;
            }
        }
        catch(QException e)
        {

        }

        //temperature
        try
        {
            maxCount = 0;
            QQueue<QString> sqls;
            while(MainWindow::temperatures.size() > 0)
            {
                //save info
                //QSqlTableModel* model = mdb->modelNoHeader("temperature");

                QList<QString> keyList = MainWindow::temperatures.keys();

                QList<QString>::const_iterator it;

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
                    while(MainWindow::temperatures[*it].size() > 1)
                    {
                        TemperatureInfo* tinfo = MainWindow::temperatures[*it].dequeue();

                        if(MainWindow::data_save_days > 0)
                        {
                            QString sql = "insert into temperature (temp,dcode,mcode,pipe,rksj) values (";
                            sql += QString::number(tinfo->temp) + ",'" + tinfo->dcode + "','" + tinfo->mcode + "'," + tinfo->pipe + ",'" + tinfo->rksj + "')";
                            sqls.enqueue(sql);
                        }
                        //qDebug() << sql;

                        if(MainWindow::server_enable && MainWindow::trans_queue.size()<MAX_QUEUE_NUM)
                        {
                            QString cinfo_str = tinfo->toString() + ";";
                            dataBuffer.append(cinfo_str.toUtf8());
                        }

                        delete tinfo;
                    }
                }

                if(dataBuffer.size() > 0)
                {
                    QByteArray compress_dataBuffer = qCompress(dataBuffer);
                    int compress_data_len = compress_dataBuffer.size();
                    sendBuffer[3] = compress_data_len / 0xFF;
                    sendBuffer[4] = compress_data_len % 0xFF;
                    sendBuffer.append(compress_dataBuffer);
                    MainWindow::trans_queue.enqueue(sendBuffer);
                }

                maxCount++;
                if(maxCount>2)
                {
                    break;
                }
            }
            if(sqls.size()>0)
            {
                mdb->execSql(sqls);
            }
        }
        catch(QException e)
        {

        }
        //sleep(50);
        msleep(500);
    }
}
