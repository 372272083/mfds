#ifndef CMIEVWAVEINFO_H
#define CMIEVWAVEINFO_H

#include <QObject>
#include <QString>
#include <QMap>

class CMIEVWaveInfo
{

public: //QMap<int,std::vector<double>> vAccWave;
    CMIEVWaveInfo(QMap<int,std::vector<double>> wave);

    std::vector<double> vAccWave[6];
    QString dcode;
    int sample_num;
    float sample_interval;
    QString sample_time;
    int run_mode;
};

#endif // CMIEVWAVEINFO_H
