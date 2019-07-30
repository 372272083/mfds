#include "cmievwaveinfo.h"

CMIEVWaveInfo::CMIEVWaveInfo(QMap<int,std::vector<double>> wave)
{
    for(int i=0;i<6;i++)
    {
        std::vector<double> wave_i = wave[i];
        int len_i = wave_i.size();
        for(int n=0;n<len_i;n++)
        {
            vAccWave[i].push_back(wave_i.at(n));
        }
    }
}
