#include "cmieewaveinfo.h"

CMIEEWaveInfo::CMIEEWaveInfo(QMap<int,std::vector<double>> wave_u,QMap<int,std::vector<double>> wave_ii)
{
    for(int i=0;i<3;i++)
    {
        std::vector<double> wave_i = wave_u[i];
        int len_i = wave_i.size();
        for(int n=0;n<len_i;n++)
        {
            volWave[i].push_back(wave_i.at(n));
        }
    }

    for(int i=0;i<3;i++)
    {
        std::vector<double> wave_i = wave_ii[i];
        int len_i = wave_i.size();
        for(int n=0;n<len_i;n++)
        {
            curWave[i].push_back(wave_i.at(n));
        }
    }
}
