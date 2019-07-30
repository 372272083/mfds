#include "cmievanalysethread.h"

#include <QMap>
#include "vibrateinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"
#include "conditioninfo.h"
#include "motorconditiontable.h"

#include "globalvariable.h"
#include "cmievwaveinfo.h"

#include "mfdslib/IO_Param.h"
#include "mfdslib/VibWaveAnsys.h"
#include "mfdslib/ElcWaveAnsys.h"

CMIEVAnalyseThread::CMIEVAnalyseThread()
{
    isLive = true;
    wi = nullptr;
}

CMIEVAnalyseThread::~CMIEVAnalyseThread()
{
    if(wi != nullptr)
    {
        delete wi;
        wi = nullptr;
    }
}

void CMIEVAnalyseThread::setData(CMIEVWaveInfo* wi)
{
    this->wi = wi;
}

bool CMIEVAnalyseThread::isAlive()
{
    return isLive;
}

void CMIEVAnalyseThread::run()
{
    //if(cal_index > 0)
    {
        int sample = wi->sample_num/wi->sample_interval;
        if(sample > 0)
        {
            //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
            for(int idx=0;idx<6;idx++)
            {
                double basefreq = 50.0;
                QVector<int> tmps = GlobalVariable::findChannelTreeIdByDeviceAndChannel(wi->dcode,QString::number(idx+1));
                if(tmps.size() == 0)
                {
                    continue;
                }
                QString mcode="";
                QVector<int>::ConstIterator tcIt;
                for(tcIt=tmps.constBegin();tcIt!=tmps.constEnd();tcIt++)
                {
                    int tmp = *tcIt;
                    if(tmp > 0)
                    {
                        tmp = GlobalVariable::findMotorTreeParentId(tmp);
                        if(tmp > 0)
                        {
                            mcode = GlobalVariable::findMotorCodeById(tmp);
                            if(GlobalVariable::motorCondition.contains(mcode))
                            {
                                float m_r = GlobalVariable::motorCondition[mcode].rotate;
                                basefreq = GlobalVariable::motorCondition[mcode].rotate / 60;
                            }
                            break;
                        }
                    }
                }

                CGlbParams *pGlbVib=new CGlbParams(basefreq,sample,wi->sample_num,4);

                SVibChnParams vibChnParams[6];
                SVibAnsyParms vibAnsyParms[6];

                double bearFeatParm[20];
                SRollBearParm rollBearParm;

                int mitreeid = GlobalVariable::findMotorTreeIndexByCode(mcode);
                MotorInfo *mi = GlobalVariable::findMotorByTreeId(mitreeid);

                if(mi != nullptr)
                {
                    rollBearParm.ballDiameter = mi->roller_d;
                    rollBearParm.contactAngle = mi->contact_angle;
                    rollBearParm.innerDiameter = mi->rin;
                    rollBearParm.outerDiameter = mi->rout;
                    rollBearParm.pitchDiameter = mi->pitch;
                    rollBearParm.nBall = mi->roller_n;

                    if(rollBearParm.nBall == -1)
                    {
                        vibAnsyParms[idx].bEnvFeat = 0;
                    }
                    else
                    {
                        vibAnsyParms[idx].bEnvFeat = 1;
                        rollBearParm.getBearFaultFeat(bearFeatParm);
                        vibAnsyParms[idx].setBearFaultFeat(bearFeatParm);
                    }
                }

                vibChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
                vibChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
                vibChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
                vibChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;

                SVibAnsyDataLib *pVibAnsyDataLib=new SVibAnsyDataLib;

                //LARGE_INTEGER str,end;
                //pGlbVib->askCurrentTime(str);
                CVibWaveAnsys *pVibWaveAnsys=new CVibWaveAnsys(pGlbVib,vibAnsyParms,vibChnParams,pVibAnsyDataLib);

                //波形
                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pAccWave[idx] == NULL)
                    {
                        if(wi->vAccWave[idx].size() > 0)
                            pVibWaveAnsys->getAnsyAccWaveByInitAccWave(wi->vAccWave[idx],idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pAccSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccSpectrum(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pAccFreqDmnFeat[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccFreqDmnFeat(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pEnvSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccEnvelope(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pEnvFreqDmnFeat[idx] == NULL)
                    {
                        pVibWaveAnsys->getEnvFreqDmnFeat(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    //if(wi->run_mode == 1 || GlobalVariable::version >= 2)
                    {
                        if(pVibAnsyDataLib->pAccTimeDmnFeat[idx] == NULL)
                        {
                            pVibWaveAnsys->getAccTimeDmnFeat(idx);
                        }
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pSpdWave[idx] == NULL)
                    {
                        pVibWaveAnsys->getAnsySpdWaveByAnsyAccWave(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pSpdSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdSpectrum(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    //if(GlobalVariable::version >= 2)
                    {
                        if(pVibAnsyDataLib->pSpdFreqDmnFeat[idx] == NULL)
                        {
                            pVibWaveAnsys->getSpdFreqDmnFeat(idx);
                        }
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    //if(wi->run_mode == 1 || GlobalVariable::version >= 2)
                    {
                        if(pVibAnsyDataLib->pSpdTimeDmnFeat[idx] == NULL)
                        {
                            pVibWaveAnsys->getSpdTimeDmnFeat(idx);
                        }
                    }
                }

                {
                    if(pVibAnsyDataLib->pAccPowSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccPowerSpectrum(idx);
                    }
                }

                {
                    if(pVibAnsyDataLib->pSpdPowSpectrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdPowerSpectrum(idx);
                    }
                }

                {
                    if(pVibAnsyDataLib->pAccCepstrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getAccCepstrum(idx);
                    }
                }

                {
                    if(pVibAnsyDataLib->pSpdCepstrum[idx] == NULL)
                    {
                        pVibWaveAnsys->getSpdCepstrum(idx);
                    }
                }

                //for(int idx=0;idx<6;idx++)
                {
                    if(pVibAnsyDataLib->pDisWave[idx] == NULL)
                    {
                        pVibWaveAnsys->getAnsyDisWaveByAnsySpdWave(idx);
                    }
                }

                //pGlbVib->askCurrentTime(end);
                //double costtime = pGlbVib->costTime(str,end);
                //qDebug() << "Aanalyse Cost Time: " << QString::number(costtime,10,4);
                //for(int idx=0;idx<6;idx++)
                {
                    QString channel = QString::number(idx+1);
                    VibrateAnyseDataLib *acc_analyse = new VibrateAnyseDataLib(pVibAnsyDataLib,idx);

                    acc_analyse->sample_time = wi->sample_time;
                    acc_analyse->sample_interval = wi->sample_interval;

                    if(wi->run_mode == 1)
                    {
                        VibrateInfo *vInfo = new VibrateInfo();
                        vInfo->vibrate_e = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->RMSValue;
                        vInfo->speed_e = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->RMSValue;
                        vInfo->position_e = 0.0;
                        vInfo->dcode = wi->dcode;
                        vInfo->pipe = channel;
                        vInfo->rksj = wi->sample_time;

                        QMutexLocker m_vibrate_lock(&GlobalVariable::vibratesglobalMutex);
                        if(GlobalVariable::vibrates.contains(wi->dcode))
                        {
                            if (GlobalVariable::vibrates[wi->dcode].contains(vInfo->pipe))
                            {
                                if(GlobalVariable::vibrates[wi->dcode][vInfo->pipe].size()<MAX_QUEUE_NUM)
                                {
                                    GlobalVariable::vibrates[wi->dcode][vInfo->pipe].enqueue(vInfo);
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
                                GlobalVariable::vibrates[wi->dcode][vInfo->pipe] = fqueue;
                            }
                        }
                        else
                        {
                            QMap<QString,QQueue<VibrateInfo*>> mchqueue;
                            QQueue<VibrateInfo*> chqueue;
                            chqueue.enqueue(vInfo);

                            mchqueue[vInfo->pipe] = chqueue;
                            GlobalVariable::vibrates[wi->dcode] = mchqueue;
                        }
                    }

                    //acc freq
                    FreqInfo *acc_fInfo = new FreqInfo();
                    QString acc_freqbuffer = "";
                    //std::vector<double> acc_freq;
                    std::vector<double>::iterator afIt;
                    for(afIt=acc_analyse->acc_freq.begin();afIt!=acc_analyse->acc_freq.end();afIt++)
                    {
                        double v = *afIt;
                        acc_freqbuffer += QString::number(v,10,4) + ",";
                    }
                    if(acc_freqbuffer.length()>0)
                    {
                        acc_freqbuffer = acc_freqbuffer.left(acc_freqbuffer.length()-1);
                    }
                    acc_fInfo->sample_freq = acc_freqbuffer;
                    acc_fInfo->dcode = wi->dcode;
                    acc_fInfo->mcode = "";
                    acc_fInfo->pipe = channel;
                    acc_fInfo->stype = 0;
                    acc_fInfo->rksj = wi->sample_time;

                    QMutexLocker m_lock(&GlobalVariable::elcglobalMutex);
                    if(GlobalVariable::freqs.contains(wi->dcode))
                    {
                        if (GlobalVariable::freqs[wi->dcode].contains(acc_fInfo->pipe))
                        {
                            if(GlobalVariable::freqs[wi->dcode][acc_fInfo->pipe].size()<MAX_QUEUE_NUM * 2)
                            {
                                GlobalVariable::freqs[wi->dcode][acc_fInfo->pipe].enqueue(acc_fInfo);
                            }
                        }
                        else
                        {
                            QQueue<FreqInfo*> fqueue;
                            fqueue.enqueue(acc_fInfo);
                            GlobalVariable::freqs[wi->dcode][acc_fInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<FreqInfo*>> dfqueue;
                        QQueue<FreqInfo*> fqueue;
                        fqueue.enqueue(acc_fInfo);
                        dfqueue[acc_fInfo->pipe] = fqueue;

                        GlobalVariable::freqs[wi->dcode] = dfqueue;
                    }

                    //spd freq
                    FreqInfo *fInfo = new FreqInfo();
                    QString freqbuffer = "";
                    for(afIt=acc_analyse->spd_freq.begin();afIt!=acc_analyse->spd_freq.end();afIt++)
                    {
                        double v = *afIt;
                        freqbuffer += QString::number(v,10,4) + ",";
                    }
                    if(freqbuffer.length()>0)
                    {
                        freqbuffer = freqbuffer.left(freqbuffer.length()-1);
                    }
                    fInfo->sample_freq = freqbuffer;
                    fInfo->dcode = wi->dcode;
                    fInfo->mcode = "";
                    fInfo->pipe = channel;
                    fInfo->stype = 1;
                    fInfo->rksj = wi->sample_time;

                    if(GlobalVariable::freqs.contains(wi->dcode))
                    {
                        if (GlobalVariable::freqs[wi->dcode].contains(fInfo->pipe))
                        {
                            if(GlobalVariable::freqs[wi->dcode][fInfo->pipe].size()<MAX_QUEUE_NUM * 2)
                            {
                                GlobalVariable::freqs[wi->dcode][fInfo->pipe].enqueue(fInfo);
                            }
                        }
                        else
                        {
                            QQueue<FreqInfo*> fqueue;
                            fqueue.enqueue(fInfo);
                            GlobalVariable::freqs[wi->dcode][fInfo->pipe] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<FreqInfo*>> dfqueue;
                        QQueue<FreqInfo*> fqueue;
                        fqueue.enqueue(fInfo);
                        dfqueue[fInfo->pipe] = fqueue;

                        GlobalVariable::freqs[wi->dcode] = dfqueue;
                    }

                    QMutexLocker m_diagnose_lock(&GlobalVariable::vibdiagnoseglobalMutex);
                    if(GlobalVariable::vibrate_analyse.contains(wi->dcode))
                    {
                        if (GlobalVariable::vibrate_analyse[wi->dcode].contains(channel))
                        {
                            if(GlobalVariable::vibrate_analyse[wi->dcode][channel].size()<MAX_QUEUE_NUM)
                            {
                                GlobalVariable::vibrate_analyse[wi->dcode][channel].enqueue(acc_analyse);
                            }
                        }
                        else
                        {
                            QQueue<VibrateAnyseDataLib*> fqueue;
                            fqueue.enqueue(acc_analyse);
                            GlobalVariable::vibrate_analyse[wi->dcode][channel] = fqueue;
                        }
                    }
                    else
                    {
                        QMap<QString,QQueue<VibrateAnyseDataLib*>> mchqueue;
                        QQueue<VibrateAnyseDataLib*> chqueue;
                        chqueue.enqueue(acc_analyse);

                        mchqueue[channel] = chqueue;
                        GlobalVariable::vibrate_analyse[wi->dcode] = mchqueue;
                    }
                }

                delete pVibWaveAnsys;
                delete pVibAnsyDataLib;

                pVibWaveAnsys = NULL;
                pVibAnsyDataLib = NULL;
                //deleteGlbParams();

                delete pGlbVib;
                pGlbVib = NULL;
            }
        }
    }

    isLive = false;
}
