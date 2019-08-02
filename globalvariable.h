#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H

#include "deviceinfo.h"
#include "motorinfo.h"
#include "treenodeinfo.h"
#include "channelinfo.h"
#include "fftparams.h"
#include "faultinfo.h"
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

#include "mfdslib/IO_Param.h"

class ChargeInfo;
class TW888Info;
class VibrateInfo;
class FreqInfo;
class WaveInfo;
class TemperatureInfo;
class ConditionInfo;
class MotorConditionTable;

class SAccTimeDmnFeat;
class SSpdTimeDmnFeat;
class SVolTimeDmnFeat;
class SCurTimeDmnFeat;

class SAccFreqDmnFeat;
class SSpdFreqDmnFeat;
class SEnvFreqDmnFeat;

class SVolFreqDmnFeat;
class SCurFreqDmnFeat;

class VibrateAnyseDataLib
{
public:
    SVibTimeDmnFeat accTimeFeat;
    SVibTimeDmnFeat spdTimeFeat;

    std::vector<double> acc_wave;
    std::vector<double> spd_wave;
    std::vector<double> dis_wave;

    std::vector<double> acc_freq;
    std::vector<double> spd_freq;
    std::vector<double> env_freq;

    std::vector<double> acc_power;
    std::vector<double> spd_power;
    std::vector<double> acc_cepstrum;
    std::vector<double> spd_cepstrum;

    SVibFreqDmnFeat accFreqFeat;
    SVibFreqDmnFeat spdFreqFeat;
    SEnvFreqDmnFeat envFreqFeat;

    QString sample_time;
    float sample_interval;

    VibrateAnyseDataLib(SVibAnsyDataLib *pVibAnsyDataLib,int idx);
};

class ElectricAnyseDataLib
{
public:

    double vVolComp[3];
    double vCurComp[3];

    SElcTimeDmnFeat volTimeFeat;
    SElcTimeDmnFeat curTimeFeat;
    SPowerAnsysFeat powerAnsysFeat;

    std::vector<double> vol_wave;
    std::vector<double> cur_wave;

    std::vector<double> vol_freq;
    std::vector<double> cur_freq;

    SElcFreqDmnFeat volFreqFeat;
    SElcFreqDmnFeat curFreqFeat;

    QString sample_time;
    float sample_interval;

    ElectricAnyseDataLib(SElcAnsyDataLib *pElcAnsyDataLib,int idx);
};

class ElectricGlobal
{
public:

    std::vector<double> vol_park_freq;
    std::vector<double> cur_park_freq;

    std::vector<double> vol_park_x;
    std::vector<double> vol_park_y;

    std::vector<double> cur_park_x;
    std::vector<double> cur_park_y;

    QString sample_time;
    float sample_interval;
    QString dcode;
    QString mcode;

    ElectricGlobal(SElcAnsyDataLib *pElcAnsyDataLib);
};

class GlobalVariable
{
public:
    GlobalVariable();
public:
    static int maxDeviceTreeId;
    static int maxMotorTreeId;
    static int maxMotorId;
    static int maxDeviceId;
    static int maxChannelId;

    static QVector<TreeNodeInfo*> motorTreeInfos;
    static QVector<DeviceInfo*> deviceInfos;
    static QVector<TreeNodeInfo*> deviceTreeInfos;
    static QVector<MotorInfo*> motorInfos;
    static QVector<ChannelInfo*> channelInfos;

    static bool com_enable; // id 1
    static bool show_enable; // id 2

    static bool virtual_data_enable; // id 3
    static bool desktop_capture_enable; // id 4

    static bool modbus_server_enable; // id 13
    static bool server_enable; // id 14
    static bool is_sync_done; // id 17

    static int wave_sample_interval; // id 5
    static int freq_sample_interval; // id 6
    static int measure_sample_interval; // id 7

    static int sample_waiting; // id 8
    static QString server_ip; // id 9
    static int server_waiting; // id 10

    static QString group_name; // id 11
    static int data_save_days; // id 12

    static int cmie_v_freq_len;
    static int cmie_v_wave_len;

    static int cmie_e_freq_len;
    static int cmie_e_wave_len;

    static QMutex chargesglobalMutex;
    static QMap<QString,QMap<QString,QQueue<ChargeInfo*>>> charges; //sample electric measure data
    static QMutex tw888chargesglobalMutex;
    static QMap<QString,QQueue<TW888Info*>> tw888charges; //sample electric measure data
    static QMutex vibratesglobalMutex;
    static QMap<QString,QMap<QString,QQueue<VibrateInfo*>>> vibrates; //sample electric vibrate data
    static QMutex temperaturesglobalMutex;
    static QMap<QString,QMap<QString,QQueue<TemperatureInfo*>>> temperatures; //sample temperature data

    static QMutex freqsglobalMutex;
    static QMap<QString,QMap<QString,QQueue<FreqInfo*>>> freqs; //sample frequency data

    static QMutex wavesglobalMutex;
    static QMap<QString,QMap<QString,QQueue<WaveInfo*>>> waves; //smaple waves data

    //////////////////////////////////////////////////////
    static QMap<QString,QMap<QString,QQueue<VibrateAnyseDataLib*>>> vibrate_analyse; //vibrate anlyse data: QString devicecode; QString channelcode;
    static QMap<QString,QMap<QString,QQueue<ElectricAnyseDataLib*>>> electric_analyse; //electric anlyse data: QString devicecode; QString channelcode;

    static QMutex vibdiagnoseglobalMutex;
    static QMap<int,QMap<QString,QMap<QString,SVibDiaFeatData>>> vibrate_diagnose; //vibrate study data:int conditino id; QString devicecode; QString channelcode;
    static QMutex elcdiagnoseglobalMutex;
    static QMap<int,QMap<QString,QMap<QString,SElcDiaFeatData>>> electric_diagnose;  //electric study data:int conditino id; QString devicecode; QString channelcode;

    static QMap<QString,QMap<QString,FaultInfo>> diagnose_result; //diagnose result data:QString motor code; QString devicecode; QString channelcode;

    //static QMap<QString,QMap<QString,QQueue<QVector<float>>>> offlinewaves;
    //static QMap<QString,QMap<QString,QQueue<QVector<float>>>> offlinespdwaves;

    static QMap<QString,QQueue<ConditionInfo>> conditions; //current device condition data:QString device code;
    static QMap<QString,MotorConditionTable> motorCondition; //current motor condition data:QString motor code;

    static QMap<QString,QVector<MotorConditionTable>> motorConditionTables; //motor condition table data:QString motor code;

    static QQueue<QByteArray> trans_queue;
    static QQueue<QByteArray> trans_queue_pri;

    static QByteArray modbus_buffer;

    static bool isMotorFire;
    static bool isSaveData;
    static bool isOnline;

    static QString dtFormat;

    static int study_num;
    static float study_time;
    static bool is_study;
    static bool is_study_again;

    static int freqlimit;

    static int waterfalldepthlimit;

    static FFTParams fft_params;

    static int version;
    static int s_t;

    static QMap<QString,int> recordwave;

    static float diagnose_rotate_diff;
    static float diagnose_u_diff;
    static float diagnose_i_diff;

    static QString rwavetime;
    static int rwaveinterval;

    static QMutex elcglobalMutex;
    static QMap<QString,QQueue<ElectricGlobal*>> elcglobaldatas;

public:
    static int findDeviceIndexById(int);
    static int findDeviceByTreeId(int);

    static QString findDeviceCodeById(int);
    static QString findDeviceTypeByCode(QString);
    static QString findDeviceTypeByTreeId(int);
    static QString findNameByCode(QString);

    static int findDeviceTreeIndexById(int);

    static QString findDeviceCodeByChannelTreeId(int);
    static QString findDeviceCodeByTreeId(int id);
    static DeviceInfo* findDeviceByCode(QString code);

    static int findMotorTreeIndexById(int);
    static int findMotorTreeParentId(int);
    static int findMotorIndexByTreeId(int id);
    static int findMotorTreeIndexByCode(QString code);
    static int findMotorRotateByCode(QString code);
    static QString findMotorCodeById(int);
    static MotorInfo* findMotorByTreeId(int);
    static QVector<QString> findMotorCodeByDeviceCode(QString code);

    static int findChannelIndexById(int);
    static QString findChannelCodeByTreeId(int);
    static QString findChannelNameByTreeId(int);
    static QString findMotorCodeByChannelId(int);
    static QVector<ChannelInfo*> findChannelsByMotorId(int);

    static QVector<int> findChannelTreeIdByDeviceAndChannel(QString,QString);

    static int findNodeType(TREENODETYPE tn);
};

#endif // GLOBALVARIABLE_H
