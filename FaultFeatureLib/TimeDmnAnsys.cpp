#include "stdafx.h"
#include "TimeDmnAnsys.h"
#include <algorithm>
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif
//////////////////////////////////////////////////
CTimeDmnAnsys::CTimeDmnAnsys(SOutputTimeDmnDatas *pOutputTimeDmnDatas0)
{
	pOutputTimeDmnDatas = pOutputTimeDmnDatas0;

	initialize();
}
CTimeDmnAnsys::~CTimeDmnAnsys()
{
}
void CTimeDmnAnsys::initialize()
{
}
int CTimeDmnAnsys::timeDomainAnsys()
{
	if (getTimeDmnFeaVaule())
	{
		return 1;
	}
	if (getAxisLocus())
	{
		return 1;
	}
	
	return 0;
}
int CTimeDmnAnsys::getTimeDmnFeaVaule()
{
	int i, j, nSize;

	nWaveData = pOutputTimeDmnDatas->outputWaveDatas.nWaveData;
	for (i = 0; i != nWaveData; i++)
	{
		DOUBLE_VCT curDatas = pOutputTimeDmnDatas->outputWaveDatas.waveData[i];
		if (curDatas.size() == 0) continue;
		
		aveValue[i] = 0.0;
		maxValue[i] = MIN_DOU_VALUE;
		minValue[i] = MAX_DOU_VALUE;
		nSize = curDatas.size();	

		double avrAbsVaule = 0.0;
		double avrSqrtVaule = 0.0;
		for (j = 0; j != nSize; j++)
		{
			aveValue[i] += curDatas[j];
			avrAbsVaule += abs(curDatas[j]);
			avrSqrtVaule += sqrt(abs(curDatas[j]));
			if (maxValue[i] < curDatas[j])
				maxValue[i] = curDatas[j];
			if (minValue[i] > curDatas[j])
				minValue[i] = curDatas[j];
		}
		aveValue[i] /= nSize;
		avrAbsVaule /= nSize;
		avrSqrtVaule /= nSize;
		avrSqrtVaule *= avrSqrtVaule;
	
        peakVueValue[i] = std::max(abs(maxValue[i]), abs(minValue[i]));
		variance[i] = 0.0;
		for (j = 0; j != nSize; j++)
		{
			variance[i] += (aveValue[i] - curDatas[j])*(aveValue[i] - curDatas[j]);
		}
		variance[i] /= nSize;

		RMS_value[i] = 0.0;
		for (j = 0; j != nSize; j++)
		{
			RMS_value[i] += curDatas[j] * curDatas[j];
		}
		RMS_value[i] = sqrt(RMS_value[i] / nSize);
	
		kurtosis[i] = 0.0;
		for (j = 0; j != nSize; j++)
		{
			kurtosis[i] += pow(curDatas[j], 4);
		}
		kurtosis[i] /= nSize;

		pOutputTimeDmnDatas->vWaveAmp[i] = peakVueValue[i];
		pOutputTimeDmnDatas->vWaveRMS[i] = RMS_value[i];
		pOutputTimeDmnDatas->vWavePkPkValue[i] = maxValue[i] - minValue[i];
		
		pOutputTimeDmnDatas->waveIndex[i] = RMS_value[i] / avrAbsVaule;
		pOutputTimeDmnDatas->peakVueIndex[i] = peakVueValue[i] / RMS_value[i];
		pOutputTimeDmnDatas->pulseIndex[i] = peakVueValue[i] / avrAbsVaule;
		pOutputTimeDmnDatas->kurtosisIndex[i] = kurtosis[i] / pow(RMS_value[i], 4);
	}
	return 0;
}
int CTimeDmnAnsys::getAxisLocus()
{
	int i, nSize;
	double curPosition;

	DOUBLE_VCT disX, disY;
	disX = pOutputTimeDmnDatas->outputWaveDatas.waveData[18];// 水平
	disY = pOutputTimeDmnDatas->outputWaveDatas.waveData[20];// 垂直
	
	pOutputTimeDmnDatas->axisLocusVct_1[0] = disX;
	pOutputTimeDmnDatas->axisLocusVct_1[1] = disY;
	nSize = disX.size();
	if (nSize == 0)
	{
		for (i = 0; i != nSize; i++)
		{
			curPosition = sqrt(disX[i] * disX[i] + disY[i] * disY[i]);
			pOutputTimeDmnDatas->axisLocusVct_2[0].push_back(curPosition);
		}
	}
		
	disX = pOutputTimeDmnDatas->outputWaveDatas.waveData[22];// 水平
	disY = pOutputTimeDmnDatas->outputWaveDatas.waveData[23];// 垂直
		
	pOutputTimeDmnDatas->axisLocusVct_1[2] = disX;
	pOutputTimeDmnDatas->axisLocusVct_1[3] = disY;
	nSize = disX.size();
	if (nSize == 0)
	{
		for (i = 0; i != nSize; i++)
		{
			curPosition = sqrt(disX[i] * disX[i] + disY[i] * disY[i]);
			pOutputTimeDmnDatas->axisLocusVct_2[1].push_back(curPosition);
		}
	}
	return 0;
}



