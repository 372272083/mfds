#ifndef CMIEEWAVEINFO_H
#define CMIEEWAVEINFO_H

#include <QObject>
#include <QString>
#include <QMap>

class CMIEEWaveInfo
{
public:
    CMIEEWaveInfo(QMap<int,std::vector<double>> wave_u,QMap<int,std::vector<double>> wave_i);

    std::vector<double> volWave[3];
    std::vector<double> curWave[3];
    QString dcode;
    QString mcode;
    int sample_num;
    float sample_interval;
    QString sample_time;
    int run_mode;
};

#endif // CMIEEWAVEINFO_H
