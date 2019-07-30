#include "globalvariable.h"

#include "chargeinfo.h"
#include "vibrateinfo.h"
#include "temperatureinfo.h"
#include "freqinfo.h"
#include "waveinfo.h"

#include "conditioninfo.h"

//#include "mfdslib/IO_Param.h"
#include "mfdslib/VibWaveAnsys.h"
#include "mfdslib/ElcWaveAnsys.h"

#include "motorconditiontable.h"

#include <QMap>

VibrateAnyseDataLib::VibrateAnyseDataLib(SVibAnsyDataLib *pVibAnsyDataLib,int idx)
{
    if(pVibAnsyDataLib->pAccTimeDmnFeat[idx] != NULL)
    {
        accTimeFeat.AmpValue = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->AmpValue;
        accTimeFeat.kurtIndex = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->kurtIndex;
        accTimeFeat.peakIndex = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->peakIndex;
        accTimeFeat.PkPkValue = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->PkPkValue;
        accTimeFeat.pulsIndex = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->pulsIndex;
        accTimeFeat.RMSValue = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->RMSValue;
        accTimeFeat.waveIndex = pVibAnsyDataLib->pAccTimeDmnFeat[idx]->waveIndex;
    }

    if(pVibAnsyDataLib->pSpdTimeDmnFeat[idx] != NULL)
    {
        spdTimeFeat.AmpValue = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->AmpValue;
        spdTimeFeat.kurtIndex = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->kurtIndex;
        spdTimeFeat.peakIndex = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->peakIndex;
        spdTimeFeat.PkPkValue = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->PkPkValue;
        spdTimeFeat.pulsIndex = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->pulsIndex;
        spdTimeFeat.RMSValue = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->RMSValue;
        spdTimeFeat.waveIndex = pVibAnsyDataLib->pSpdTimeDmnFeat[idx]->waveIndex;
    }

    int len = 0;
    if(pVibAnsyDataLib->pAccWave[idx] != NULL)
    {
        len = pVibAnsyDataLib->pAccWave[idx]->size();
        for(int i=0;i<len;i++)
        {
            acc_wave.push_back(pVibAnsyDataLib->pAccWave[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pSpdWave[idx] != NULL)
    {
        len = pVibAnsyDataLib->pSpdWave[idx]->size();
        for(int i=0;i<len;i++)
        {
            spd_wave.push_back(pVibAnsyDataLib->pSpdWave[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pDisWave[idx] != NULL)
    {
        len = pVibAnsyDataLib->pDisWave[idx]->size();
        for(int i=0;i<len;i++)
        {
            dis_wave.push_back(pVibAnsyDataLib->pDisWave[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pAccSpectrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pAccSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            acc_freq.push_back(pVibAnsyDataLib->pAccSpectrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pSpdSpectrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pSpdSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            spd_freq.push_back(pVibAnsyDataLib->pSpdSpectrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pEnvSpectrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pEnvSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            env_freq.push_back(pVibAnsyDataLib->pEnvSpectrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pAccPowSpectrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pAccPowSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            acc_power.push_back(pVibAnsyDataLib->pAccPowSpectrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pSpdPowSpectrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pSpdPowSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            spd_power.push_back(pVibAnsyDataLib->pSpdPowSpectrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pAccCepstrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pAccCepstrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            acc_cepstrum.push_back(pVibAnsyDataLib->pAccCepstrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pSpdCepstrum[idx] != NULL)
    {
        len = pVibAnsyDataLib->pSpdCepstrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            spd_cepstrum.push_back(pVibAnsyDataLib->pSpdCepstrum[idx]->at(i));
        }
    }

    if(pVibAnsyDataLib->pAccFreqDmnFeat[idx] != NULL)
    {
        for(int i=0;i<11;i++)
        {
            accFreqFeat.vibMultFrq[i] = pVibAnsyDataLib->pAccFreqDmnFeat[idx]->vibMultFrq[i];
        }
    }

    if(pVibAnsyDataLib->pSpdFreqDmnFeat[idx] != NULL)
    {
        for(int i=0;i<11;i++)
        {
            spdFreqFeat.vibMultFrq[i] = pVibAnsyDataLib->pSpdFreqDmnFeat[idx]->vibMultFrq[i];
        }
    }

    if(pVibAnsyDataLib->pEnvFreqDmnFeat[idx] != NULL)
    {
        for(int i=0;i<20;i++)
        {
            envFreqFeat.envMultFrq[i] = pVibAnsyDataLib->pEnvFreqDmnFeat[idx]->envMultFrq[i];
        }
    }
}

ElectricAnyseDataLib::ElectricAnyseDataLib(SElcAnsyDataLib *pElcAnsyDataLib,int idx)
{
    if(pElcAnsyDataLib->vVolComp[idx][0] != -1)
    {
        vVolComp[0] = pElcAnsyDataLib->vVolComp[idx][0];
        vVolComp[1] = pElcAnsyDataLib->vVolComp[idx][1];
        vVolComp[2] = pElcAnsyDataLib->vVolComp[idx][2];
    }

    if(pElcAnsyDataLib->vCurComp[idx][0] != -1)
    {
        vCurComp[0] = pElcAnsyDataLib->vCurComp[idx][0];
        vCurComp[1] = pElcAnsyDataLib->vCurComp[idx][1];
        vCurComp[2] = pElcAnsyDataLib->vCurComp[idx][2];
    }

    if(pElcAnsyDataLib->pVolTimeDmnFeat[idx] != NULL)
    {
        volTimeFeat.AmpValue = pElcAnsyDataLib->pVolTimeDmnFeat[idx]->AmpValue;
        volTimeFeat.PkPkValue = pElcAnsyDataLib->pVolTimeDmnFeat[idx]->PkPkValue;
        volTimeFeat.RMSValue = pElcAnsyDataLib->pVolTimeDmnFeat[idx]->RMSValue;
    }

    if(pElcAnsyDataLib->pCurTimeDmnFeat[idx] != NULL)
    {
        curTimeFeat.AmpValue = pElcAnsyDataLib->pCurTimeDmnFeat[idx]->AmpValue;
        curTimeFeat.PkPkValue = pElcAnsyDataLib->pCurTimeDmnFeat[idx]->PkPkValue;
        curTimeFeat.RMSValue = pElcAnsyDataLib->pCurTimeDmnFeat[idx]->RMSValue;
    }

    int len = 0;
    if(pElcAnsyDataLib->pVolWave[idx] != NULL)
    {
        len = pElcAnsyDataLib->pVolWave[idx]->size();
        for(int i=0;i<len;i++)
        {
            vol_wave.push_back(pElcAnsyDataLib->pVolWave[idx]->at(i));
        }
    }

    if(pElcAnsyDataLib->pCurWave[idx] != NULL)
    {
        len = pElcAnsyDataLib->pCurWave[idx]->size();
        for(int i=0;i<len;i++)
        {
            cur_wave.push_back(pElcAnsyDataLib->pCurWave[idx]->at(i));
        }
    }

    if(pElcAnsyDataLib->pVolSpectrum[idx] != NULL)
    {
        len = pElcAnsyDataLib->pVolSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            vol_freq.push_back(pElcAnsyDataLib->pVolSpectrum[idx]->at(i));
        }
    }

    if(pElcAnsyDataLib->pCurSpectrum[idx] != NULL)
    {
        len = pElcAnsyDataLib->pCurSpectrum[idx]->size();
        for(int i=0;i<len;i++)
        {
            cur_freq.push_back(pElcAnsyDataLib->pCurSpectrum[idx]->at(i));
        }
    }

    if(pElcAnsyDataLib->pVolFreqDmnFeat[idx] != NULL)
    {
        for(int i=0;i<11;i++)
        {
            volFreqFeat.elcMultFrq[i] = pElcAnsyDataLib->pVolFreqDmnFeat[idx]->elcMultFrq[i];
        }
    }

    if(pElcAnsyDataLib->pCurFreqDmnFeat[idx] != NULL)
    {
        for(int i=0;i<11;i++)
        {
            curFreqFeat.elcMultFrq[i] = pElcAnsyDataLib->pCurFreqDmnFeat[idx]->elcMultFrq[i];
        }
    }

    if(pElcAnsyDataLib->pPowerAnsysFeat[idx] != NULL)
    {
        powerAnsysFeat.actPower = pElcAnsyDataLib->pPowerAnsysFeat[idx]->actPower;
        powerAnsysFeat.appPower = pElcAnsyDataLib->pPowerAnsysFeat[idx]->appPower;
        powerAnsysFeat.cosf = pElcAnsyDataLib->pPowerAnsysFeat[idx]->cosf;
        powerAnsysFeat.reactPower = pElcAnsyDataLib->pPowerAnsysFeat[idx]->reactPower;
    }
}

ElectricGlobal::ElectricGlobal(SElcAnsyDataLib *pElcAnsyDataLib)
{
    if(pElcAnsyDataLib->pVol_Dq[0] != NULL)
    {
        int len = pElcAnsyDataLib->pVol_Dq[0]->size();

        for(int i=0;i<len;i++)
        {
            vol_park_x.push_back(pElcAnsyDataLib->pVol_Dq[0]->at(i));
        }

        len = pElcAnsyDataLib->pVol_Dq[1]->size();
        for(int i=0;i<len;i++)
        {
            vol_park_y.push_back(pElcAnsyDataLib->pVol_Dq[1]->at(i));
        }

        len = pElcAnsyDataLib->pCur_Dq[0]->size();

        for(int i=0;i<len;i++)
        {
            cur_park_x.push_back(pElcAnsyDataLib->pCur_Dq[0]->at(i));
        }

        len = pElcAnsyDataLib->pCur_Dq[1]->size();
        for(int i=0;i<len;i++)
        {
            cur_park_y.push_back(pElcAnsyDataLib->pCur_Dq[1]->at(i));
        }

        /*
        len = pElcAnsyDataLib->pVolSpectrum_DQ->size();
        for(int i=0;i<len;i++)
        {
            vol_park_freq.push_back(pElcAnsyDataLib->pVolSpectrum_DQ->at(i));
        }

        len = pElcAnsyDataLib->pCurSpectrum_DQ->size();
        for(int i=0;i<len;i++)
        {
            cur_park_freq.push_back(pElcAnsyDataLib->pCurSpectrum_DQ->at(i));
        }
        */
    }
}

int GlobalVariable::maxDeviceTreeId = 0;
int GlobalVariable::maxMotorTreeId = 0;
int GlobalVariable::maxChannelId = 0;
int GlobalVariable::maxMotorId = 0;
int GlobalVariable::maxDeviceId = 0;

QVector<TreeNodeInfo*> GlobalVariable::motorTreeInfos;
QVector<DeviceInfo*> GlobalVariable::deviceInfos;
QVector<MotorInfo*> GlobalVariable::motorInfos;
QVector<ChannelInfo*> GlobalVariable::channelInfos;
QVector<TreeNodeInfo*> GlobalVariable::deviceTreeInfos;

bool GlobalVariable::com_enable = true;
bool GlobalVariable::show_enable = true;

bool GlobalVariable::virtual_data_enable = false;
bool GlobalVariable::desktop_capture_enable = false;

bool GlobalVariable::modbus_server_enable = false;
bool GlobalVariable::server_enable = false;
bool GlobalVariable::is_sync_done = false;

int GlobalVariable::freq_sample_interval = 10;
int GlobalVariable::wave_sample_interval = 1800;
int GlobalVariable::measure_sample_interval = 2;

int GlobalVariable::cmie_v_freq_len = 12296;
int GlobalVariable::cmie_v_wave_len = 196616;

int GlobalVariable::cmie_e_freq_len = 12296;
int GlobalVariable::cmie_e_wave_len = 196616;

int GlobalVariable::sample_waiting = 20;
QString GlobalVariable::server_ip = "127.0.0.1";
int GlobalVariable::server_waiting = 50;

QString GlobalVariable::group_name = "";
int GlobalVariable::data_save_days = 3;

bool GlobalVariable::isMotorFire = false;
//isSaveData
bool GlobalVariable::isSaveData = false;
bool GlobalVariable::isOnline = true;

QMutex GlobalVariable::chargesglobalMutex;
QMap<QString,QMap<QString,QQueue<ChargeInfo*>>> GlobalVariable::charges;
QMutex GlobalVariable::vibratesglobalMutex;
QMap<QString,QMap<QString,QQueue<VibrateInfo*>>> GlobalVariable::vibrates;
QMutex GlobalVariable::temperaturesglobalMutex;
QMap<QString,QMap<QString,QQueue<TemperatureInfo*>>> GlobalVariable::temperatures;

QMutex GlobalVariable::freqsglobalMutex;
QMap<QString,QMap<QString,QQueue<FreqInfo*>>> GlobalVariable::freqs;

QMutex GlobalVariable::wavesglobalMutex;
QMap<QString,QMap<QString,QQueue<WaveInfo*>>> GlobalVariable::waves;

QMutex GlobalVariable::vibdiagnoseglobalMutex;
QMap<QString,QMap<QString,QQueue<VibrateAnyseDataLib*>>> GlobalVariable::vibrate_analyse;
QMutex GlobalVariable::elcdiagnoseglobalMutex;
QMap<QString,QMap<QString,QQueue<ElectricAnyseDataLib*>>> GlobalVariable::electric_analyse;

QMap<int,QMap<QString,QMap<QString,SVibDiaFeatData>>> GlobalVariable::vibrate_diagnose;
QMap<int,QMap<QString,QMap<QString,SElcDiaFeatData>>> GlobalVariable::electric_diagnose;

QMap<QString,QMap<QString,FaultInfo>> GlobalVariable::diagnose_result;

//QMap<QString,QMap<QString,QQueue<QVector<float>>>> GlobalVariable::offlinewaves;
//QMap<QString,QMap<QString,QQueue<QVector<float>>>> GlobalVariable::offlinespdwaves;

QMap<QString,QQueue<ConditionInfo>> GlobalVariable::conditions;
QMap<QString,MotorConditionTable> GlobalVariable::motorCondition;

QMap<QString,QVector<MotorConditionTable>> GlobalVariable::motorConditionTables;

QQueue<QByteArray> GlobalVariable::trans_queue;
QQueue<QByteArray> GlobalVariable::trans_queue_pri;

QByteArray GlobalVariable::modbus_buffer;

QString GlobalVariable::dtFormat = "yyyy-MM-dd hh:mm:ss";

int GlobalVariable::study_num = 500;
float GlobalVariable::study_time = 1.0;
bool GlobalVariable::is_study = false;
bool GlobalVariable::is_study_again = false;

int GlobalVariable::freqlimit = 1024;
int GlobalVariable::waterfalldepthlimit = 10;

FFTParams GlobalVariable::fft_params;

int GlobalVariable::version = 1;
int GlobalVariable::s_t = 0; //0 all; 1 vibrate; 2 electric; 3 temperature; 10 vibrate contg wave

QMap<QString,int> GlobalVariable::recordwave;

float GlobalVariable::diagnose_rotate_diff = 0.125;
float GlobalVariable::diagnose_u_diff = 0.125;
float GlobalVariable::diagnose_i_diff = 0.125;

QString GlobalVariable::rwavetime;
int GlobalVariable::rwaveinterval = 0;

QMutex GlobalVariable::elcglobalMutex;
QMap<QString,QQueue<ElectricGlobal*>> GlobalVariable::elcglobaldatas;

GlobalVariable::GlobalVariable()
{

}

int GlobalVariable::findDeviceIndexById(int id)
{
    int index = 0;
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

int GlobalVariable::findDeviceByTreeId(int id)
{
    int index = 0;
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

int GlobalVariable::findDeviceTreeIndexById(int id)
{
    int index = 0;
    QVector<TreeNodeInfo*>::ConstIterator cIt;
    for(cIt=deviceTreeInfos.constBegin();cIt != deviceTreeInfos.constEnd();cIt++)
    {
        TreeNodeInfo* item = *cIt;
        if(item->id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

int GlobalVariable::findMotorTreeIndexById(int id)
{
    int index = 0;
    QVector<TreeNodeInfo*>::ConstIterator cIt;
    for(cIt=motorTreeInfos.constBegin();cIt != motorTreeInfos.constEnd();cIt++)
    {
        TreeNodeInfo* item = *cIt;
        if(item->id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

int GlobalVariable::findMotorRotateByCode(QString code)
{
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=motorInfos.constBegin();cIt != motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->code.compare(code,Qt::CaseInsensitive) == 0)
        {
            return item->rotate;
        }
    }

    return 0;
}

int GlobalVariable::findMotorTreeIndexByCode(QString code)
{
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=motorInfos.constBegin();cIt != motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->code.compare(code,Qt::CaseInsensitive) == 0)
        {
            return item->tree_id;
        }
    }

    return 0;
}

MotorInfo* GlobalVariable::findMotorByTreeId(int id)
{
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=motorInfos.constBegin();cIt != motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item;
        }
    }

    return nullptr;
}

int GlobalVariable::findMotorTreeParentId(int id)
{
    int index = 0;
    QVector<TreeNodeInfo*>::ConstIterator cIt;
    for(cIt=motorTreeInfos.constBegin();cIt != motorTreeInfos.constEnd();cIt++)
    {
        TreeNodeInfo* item = *cIt;
        if(item->id == id)
        {
            return item->pid;
        }
    }

    return 0;
}

QVector<QString> GlobalVariable::findMotorCodeByDeviceCode(QString code)
{
    QVector<QString> result;
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->device_code.compare(code,Qt::CaseInsensitive) == 0)
        {
            result.append(item->motor_code);
        }
    }

    return result;
}

int GlobalVariable::findMotorIndexByTreeId(int id)
{
    int index = 0;
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=motorInfos.constBegin();cIt != motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

QString GlobalVariable::findMotorCodeById(int id)
{
    QString result="";
    QVector<MotorInfo*>::ConstIterator cIt;
    for(cIt=motorInfos.constBegin();cIt != motorInfos.constEnd();cIt++)
    {
        MotorInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->code;
        }
    }

    return result;
}

int GlobalVariable::findChannelIndexById(int id)
{
    int index = 0;
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->id == id)
        {
            return index;
        }
        index++;
    }

    return -1;
}

QString GlobalVariable::findChannelCodeByTreeId(int id)
{
    QString result="";
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->channel_code;
        }
    }

    return result;
}

QString GlobalVariable::findChannelNameByTreeId(int id)
{
    QString result="";
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->title;
        }
    }

    return result;
}

QString GlobalVariable::findMotorCodeByChannelId(int id)
{
    QString result="";
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->motor_code;
        }
    }

    return result;
}

QString GlobalVariable::findDeviceCodeById(int id)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->id == id)
        {
            return item->deviceCode;
        }
    }

    return "";
}

DeviceInfo* GlobalVariable::findDeviceByCode(QString code)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->deviceCode.compare(code,Qt::CaseInsensitive) == 0)
        {
            return item;
        }
    }

    return nullptr;
}

QString GlobalVariable::findDeviceTypeByCode(QString code)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->deviceCode.compare(code,Qt::CaseInsensitive) == 0)
        {
            return item->deviceType;
        }
    }

    return "";
}

QString GlobalVariable::findDeviceTypeByTreeId(int id)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->deviceType;
        }
    }

    return "";
}

QString GlobalVariable::findDeviceCodeByChannelTreeId(int id)
{
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->device_code;
        }
    }

    return "";
}

QString GlobalVariable::findDeviceCodeByTreeId(int id)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->tree_id == id)
        {
            return item->deviceCode;
        }
    }

    return "";
}

QString GlobalVariable::findNameByCode(QString code)
{
    QVector<DeviceInfo*>::ConstIterator cIt;
    for(cIt=deviceInfos.constBegin();cIt != deviceInfos.constEnd();cIt++)
    {
        DeviceInfo* item = *cIt;
        if(item->deviceCode.compare(code,Qt::CaseInsensitive) == 0)
        {
            return item->title;
        }
    }

    return "";
}

QVector<ChannelInfo*> GlobalVariable::findChannelsByMotorId(int id)
{
    QList<int> channel_treeids;

    QVector<TreeNodeInfo*>::ConstIterator cIt;
    for(cIt=motorTreeInfos.constBegin();cIt != motorTreeInfos.constEnd();cIt++)
    {
        TreeNodeInfo* item = *cIt;
        if(item->pid == id)
        {
            channel_treeids.append(item->id);
        }
    }

    QVector<ChannelInfo*> result;
    QList<int>::ConstIterator it;
    for(it=channel_treeids.constBegin();it!=channel_treeids.constEnd();it++)
    {
        int tree_id = *it;
        QVector<ChannelInfo*>::ConstIterator ciIt;
        for(ciIt=channelInfos.constBegin();ciIt != channelInfos.constEnd();ciIt++)
        {
            ChannelInfo* item = *ciIt;
            if(item->tree_id == tree_id)
            {
                result.push_back(item);
            }
        }
    }

    return result;
}

QVector<int> GlobalVariable::findChannelTreeIdByDeviceAndChannel(QString dcode,QString channel)
{
    QVector<int> result;
    QVector<ChannelInfo*>::ConstIterator cIt;
    for(cIt=channelInfos.constBegin();cIt != channelInfos.constEnd();cIt++)
    {
        ChannelInfo* item = *cIt;
        if(dcode.compare(item->device_code,Qt::CaseInsensitive) == 0 && channel.compare(item->channel_code,Qt::CaseInsensitive) == 0)
        {
            result.push_back(item->tree_id);
        }
    }

    return result;
}

int GlobalVariable::findNodeType(TREENODETYPE tn)
{
    int result = 0;
    switch (tn) {
        case MEASURE:
        case MEASURE_SPEED:
        case MEASURE_POS:
        case WAVE:
        case FREQENCY:
        case WAVE_A:
        case SPEED_WAVE_A:
        case WAVE_FREQ_A:
        case SPEED_FREQ_A:
        case EN_FREQ_A:
        case DIS_WAVE_A:
        case WATERFALL_ACC:
        case WATERFALL_SPD:
        case PATH_TRACKING:
        case ACC_POWER:
        case SPD_POWER:
        case ACC_Cepstrum:
        case SPD_Cepstrum:
        case ACC_FREQ_HX:
        case ACC_FREQ_1X:
        case ACC_FREQ_2X:
        case ACC_FREQ_3X:
        case ACC_FREQ_4X:
        case ACC_FREQ_5X:
        case ACC_FREQ_6X:
        case ACC_FREQ_7X:
        case ACC_FREQ_8X:
        case ACC_FREQ_9X:
        case ACC_FREQ_10X:
        case SPD_FREQ_HX:
        case SPD_FREQ_1X:
        case SPD_FREQ_2X:
        case SPD_FREQ_3X:
        case SPD_FREQ_4X:
        case SPD_FREQ_5X:
        case SPD_FREQ_6X:
        case SPD_FREQ_7X:
        case SPD_FREQ_8X:
        case SPD_FREQ_9X:
        case SPD_FREQ_10X:
            result = 1;
            break;
        case UMEASURE:
        case IMEASURE:
        case UWAVE:
        case IWAVE:
        case UFREQENCY:
        case IFREQENCY:
        case WATERFALL_VOL:
        case WATERFALL_CUR:
        case ACTPOWER:
        case REACTPOWER:
        case APPPOWER:
        case COSF:
        case U_P_SEQUENCE:
        case U_N_SEQUENCE:
        case U_Z_SEQUENCE:
        case I_P_SEQUENCE:
        case I_N_SEQUENCE:
        case I_Z_SEQUENCE:
        case PARK_TRANSFORMATION:
        case VOL_FREQ_HX:
        case VOL_FREQ_1X:
        case VOL_FREQ_2X:
        case VOL_FREQ_3X:
        case VOL_FREQ_4X:
        case VOL_FREQ_5X:
        case VOL_FREQ_6X:
        case VOL_FREQ_7X:
        case VOL_FREQ_8X:
        case VOL_FREQ_9X:
        case VOL_FREQ_10X:
        case CUR_FREQ_HX:
        case CUR_FREQ_1X:
        case CUR_FREQ_2X:
        case CUR_FREQ_3X:
        case CUR_FREQ_4X:
        case CUR_FREQ_5X:
        case CUR_FREQ_6X:
        case CUR_FREQ_7X:
        case CUR_FREQ_8X:
        case CUR_FREQ_9X:
        case CUR_FREQ_10X:
            result = 2;
            break;
        case TEMP_MEASURE:
            result = 3;
            break;
        default:
            break;
    }

    return result;
}
