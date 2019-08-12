#include "cmievwaveinfo.h"

CMIEVWaveInfo::CMIEVWaveInfo(QQueue<QMap<int,std::vector<double>>> waves)
{
    while (waves.size()>0) {
        QMap<int,std::vector<double>> wave = waves.back();
        waves.pop_back();

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
}
