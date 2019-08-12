#ifndef CMIEVWAVEINFO_H
#define CMIEVWAVEINFO_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QQueue>

class CMIEVWaveInfo
{

public: //QMap<int,std::vector<double>> vAccWave;
    CMIEVWaveInfo(QQueue<QMap<int,std::vector<double>>> waves);

    std::vector<double> vAccWave[6];
    QString dcode;
    int sample_num;
    float sample_interval;
    QString sample_time;
    int run_mode;
};

#endif // CMIEVWAVEINFO_H
