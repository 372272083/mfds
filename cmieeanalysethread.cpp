#include "cmieeanalysethread.h"

#include <QMap>
#include <QMutexLocker>
#include "vibrateinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"
#include "conditioninfo.h"
#include "motorconditiontable.h"

#include "globalvariable.h"
#include "cmieewaveinfo.h"

#include "mfdslib/IO_Param.h"
#include "mfdslib/VibWaveAnsys.h"
#include "mfdslib/ElcWaveAnsys.h"

CMIEEAnalyseThread::CMIEEAnalyseThread()
{
    isLive = true;
    wi = nullptr;
}

CMIEEAnalyseThread::~CMIEEAnalyseThread()
{
    if(wi != nullptr)
    {
        delete wi;
        wi = nullptr;
    }
}

void CMIEEAnalyseThread::setData(CMIEEWaveInfo* wi)
{
    this->wi = wi;
}

bool CMIEEAnalyseThread::isAlive()
{
    return isLive;
}

void CMIEEAnalyseThread::run()
{
    //if((run_mode == 1 && cal_index > 0) || GlobalVariable::is_study) //wave mode calculate efficency value
    {
        int sample = wi->sample_num/wi->sample_interval;
        //SGlbParams *glbParams = new SGlbParams(sample,intervals[0]);
        double basefreq = 50.0;
        QString mcode = wi->mcode;
        if(mcode.length() == 0)
        {
            return;
        }

        CGlbParams *pGlbElc=new CGlbParams(basefreq,sample,wi->sample_num,4);

        SElcChnParams elcChnParams[6];
        SElcAnsyParms elcAnsyParms[6];

        for(int idx=0;idx<3;idx++)
        {
            elcChnParams[idx].windowType = (EnWindowType)GlobalVariable::fft_params.fft_w;
            elcChnParams[idx].filterFreq[0] = GlobalVariable::fft_params.filter_lower;
            elcChnParams[idx].filterFreq[1] = GlobalVariable::fft_params.filter_up;
            elcChnParams[idx].filterType = (EnFilterType)GlobalVariable::fft_params.fft_filter;
        }

        SElcAnsyDataLib *pElcAnsyDataLib=new SElcAnsyDataLib;
        CElcWaveAnsys *pElcWaveAnsys=new CElcWaveAnsys(pGlbElc,elcAnsyParms,elcChnParams,pElcAnsyDataLib);

        for(int idx=0;idx!=3;idx++)
        {
            if(wi->volWave[idx].size() != 0)
            {
                if(pElcAnsyDataLib->pVolWave[idx] == NULL)
                {
                    pElcWaveAnsys->getAnsyVolWaveByInitVolWave(wi->volWave[idx],idx);
                }
            }
            if(wi->curWave[idx].size() != 0)
            {
                if(pElcAnsyDataLib->pCurWave[idx] == NULL)
                {
                    pElcWaveAnsys->getAnsyCurWaveByInitCurWave(wi->curWave[idx],idx);
                }
            }
            if(pElcAnsyDataLib->pVolWave[idx] != NULL)
            {
                //if(GlobalVariable::version >= 2)
                {
                    if(pElcAnsyDataLib->pVolTimeDmnFeat[idx] == NULL)
                    {
                        pElcWaveAnsys->getVolTimeDmnFeat(idx);
                    }
                }
            }
            if(pElcAnsyDataLib->pCurWave[idx] != NULL)
            {
                //if(GlobalVariable::version >= 2)
                {
                    if(pElcAnsyDataLib->pCurTimeDmnFeat[idx] == NULL)
                    {
                        pElcWaveAnsys->getCurTimeDmnFeat(idx);
                    }
                }
            }
            if(pElcAnsyDataLib->pVolWave[idx] != NULL && pElcAnsyDataLib->pCurWave[idx] != NULL)
            {
                if(pElcAnsyDataLib->pPowerAnsysFeat[idx] == NULL)
                {
                    pElcWaveAnsys->getPowerAnsyFeat(idx);
                }
            }
            if(pElcAnsyDataLib->pVolWave[idx] != NULL)
            {
                if(pElcAnsyDataLib->pVolSpectrum[idx] == NULL)
                {
                    pElcWaveAnsys->getVolSpectrum(idx);
                }
            }
            if(pElcAnsyDataLib->pCurWave[idx] != NULL)
            {
                if(pElcAnsyDataLib->pCurSpectrum[idx] == NULL)
                {
                    pElcWaveAnsys->getCurSpectrum(idx);
                }
            }
            if(pElcAnsyDataLib->pVolSpectrum[idx] != NULL)
            {
                if(pElcAnsyDataLib->pVolFreqDmnFeat[idx] == NULL)
                {
                    pElcWaveAnsys->getVolFreqDmnFeat(idx);
                }
            }
            if(pElcAnsyDataLib->pCurSpectrum[idx] != NULL)
            {
                if(pElcAnsyDataLib->pCurFreqDmnFeat[idx] == NULL)
                {
                    pElcWaveAnsys->getCurFreqDmnFeat(idx);
                }
            }

            if(pElcAnsyDataLib->pVolSpectrum[0] != NULL && pElcAnsyDataLib->pVolSpectrum[1] != NULL && pElcAnsyDataLib->pVolSpectrum[2] != NULL)
            {
                if(pElcAnsyDataLib->vVolComp[0][0] == -1)
                {
                    pElcWaveAnsys->getVolComponent();
                }
            }

            if(pElcAnsyDataLib->pCurSpectrum[0] != NULL && pElcAnsyDataLib->pCurSpectrum[1] != NULL && pElcAnsyDataLib->pCurSpectrum[2] != NULL)
            {
                if(pElcAnsyDataLib->vCurComp[0][0] == -1)
                {
                    pElcWaveAnsys->getCurComponent();
                }
            }

            /*
            if(pElcAnsyDataLib->pVolWave[0] != NULL &&
               pElcAnsyDataLib->pVolWave[1] != NULL &&
               pElcAnsyDataLib->pVolWave[2] != NULL &&
               pElcAnsyDataLib->pVolSpectrum_DQ == NULL)
            {
                pElcWaveAnsys->getVolDqComponent();
                pElcWaveAnsys->getVolSpectrum_DQ();
            }

            if(pElcAnsyDataLib->pCurWave[0] != NULL&&
               pElcAnsyDataLib->pCurWave[1] != NULL&&
               pElcAnsyDataLib->pCurWave[2] != NULL&&
               pElcAnsyDataLib->pCurSpectrum_DQ == NULL)
            {
                pElcWaveAnsys->getCurDqComponent();
                pElcWaveAnsys->getCurSpectrum_DQ();
            }
            */
        }

        QMutexLocker m_elc_lock(&GlobalVariable::elcglobalMutex);
        if(mcode.length()>0)
        {
            ElectricGlobal *eg = new ElectricGlobal(pElcAnsyDataLib);
            eg->sample_time = wi->sample_time;
            eg->sample_interval = wi->sample_interval;
            eg->dcode = wi->dcode;
            eg->mcode = mcode;

            if(GlobalVariable::elcglobaldatas.contains(mcode))
            {
                GlobalVariable::elcglobaldatas[mcode].push_back(eg);
            }
            else
            {
                QQueue<ElectricGlobal*> es;
                es.push_back(eg);
                GlobalVariable::elcglobaldatas[mcode] = es;
            }
        }
        QMutexLocker m_lock(&GlobalVariable::freqsglobalMutex);
        for(int idx=0;idx<3;idx++)
        {
            QString channel = QString::number(idx+1);
            ElectricAnyseDataLib *elc_analyse = new ElectricAnyseDataLib(pElcAnsyDataLib,idx);

            elc_analyse->sample_time = wi->sample_time;
            elc_analyse->sample_interval = wi->sample_interval;

            FreqInfo* vol_fInfo = new FreqInfo();
            QString vol_freqbuffer = "";
            //std::vector<double> acc_freq;
            std::vector<double>::iterator afIt;
            for(afIt=elc_analyse->vol_freq.begin();afIt!=elc_analyse->vol_freq.end();afIt++)
            {
                double v = *afIt;
                vol_freqbuffer += QString::number(v,10,4) + ",";
            }
            if(vol_freqbuffer.length()>0)
            {
                vol_freqbuffer = vol_freqbuffer.left(vol_freqbuffer.length()-1);
            }
            vol_fInfo->sample_freq = vol_freqbuffer;
            vol_fInfo->dcode = wi->dcode;
            vol_fInfo->mcode = "";
            vol_fInfo->rksj = wi->sample_time;
            vol_fInfo->pipe = channel;
            vol_fInfo->stype = 0;

            if(GlobalVariable::freqs.contains(wi->dcode))
            {
                if (GlobalVariable::freqs[wi->dcode].contains(vol_fInfo->pipe))
                {
                    if(GlobalVariable::freqs[wi->dcode][vol_fInfo->pipe].size()<MAX_QUEUE_NUM*2)
                    {
                        GlobalVariable::freqs[wi->dcode][vol_fInfo->pipe].enqueue(vol_fInfo);
                    }
                }
                else
                {
                    QQueue<FreqInfo*> fqueue;
                    fqueue.enqueue(vol_fInfo);
                    GlobalVariable::freqs[wi->dcode][vol_fInfo->pipe] = fqueue;
                }
            }
            else
            {
                QMap<QString,QQueue<FreqInfo*>> dfqueue;
                QQueue<FreqInfo*> fqueue;
                fqueue.enqueue(vol_fInfo);
                dfqueue[vol_fInfo->pipe] = fqueue;

                GlobalVariable::freqs[wi->dcode] = dfqueue;
            }

            FreqInfo* fInfo = new FreqInfo();
            QString freqbuffer = "";
            for(afIt=elc_analyse->cur_freq.begin();afIt!=elc_analyse->cur_freq.end();afIt++)
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
            fInfo->rksj = wi->sample_time;
            fInfo->pipe = channel;
            fInfo->stype = 1;

            if(GlobalVariable::freqs.contains(wi->dcode))
            {
                if (GlobalVariable::freqs[wi->dcode].contains(fInfo->pipe))
                {
                    if(GlobalVariable::freqs[wi->dcode][fInfo->pipe].size()<MAX_QUEUE_NUM*2)
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

            QMutexLocker m_diagnose_lock(&GlobalVariable::elcdiagnoseglobalMutex);
            if(GlobalVariable::electric_analyse.contains(wi->dcode))
            {
                if (GlobalVariable::electric_analyse[wi->dcode].contains(channel))
                {
                    if(GlobalVariable::electric_analyse[wi->dcode][channel].size()<MAX_QUEUE_NUM)
                    {
                        GlobalVariable::electric_analyse[wi->dcode][channel].enqueue(elc_analyse);
                    }
                }
                else
                {
                    QQueue<ElectricAnyseDataLib*> fqueue;
                    fqueue.enqueue(elc_analyse);
                    GlobalVariable::electric_analyse[wi->dcode][channel] = fqueue;
                }
            }
            else
            {
                QMap<QString,QQueue<ElectricAnyseDataLib*>> mchqueue;
                QQueue<ElectricAnyseDataLib*> chqueue;
                chqueue.enqueue(elc_analyse);

                mchqueue[channel] = chqueue;
                GlobalVariable::electric_analyse[wi->dcode] = mchqueue;
            }
        }

        delete pElcWaveAnsys;
        delete pElcAnsyDataLib;

        pElcWaveAnsys = NULL;
        pElcAnsyDataLib = NULL;
        //deleteGlbParams();

        delete pGlbElc;
        pGlbElc = NULL;

    }

    isLive = false;
}
