#include "stdafx.h"
#include "globals.h"
#include "DFT_ANSYS.h"
#include "FFT_ANSYS.h"
#include "RotateFreqAnsys.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CRotateFreqAnsys::CRotateFreqAnsys(SDiagnosisInPutData *pInputDatas0,
								   SOutputWaveDatas outputWaveDatas0,
								   double *rotaFreq0)
{
	pInputDatas = pInputDatas0;
	outputWaveDatas = outputWaveDatas0;
	rotaFreq = rotaFreq0;
	
	s_motor[0] = 0.0; s_motor[1] = 0.05;
	fs = pInputDatas->diagnosisParms.systemParams.fs;
	R_motor = pInputDatas->motorLib_ID.nameplateData.motorSlot;
	p_motor = pInputDatas->motorLib_ID.nameplateData.polePairs;
	windowType = pInputDatas->diagnosisParms.systemParams.windowType;

	nLenData = outputWaveDatas.waveData[0].size() / 2;
    //_ASSERT(nLenData == 8192);
	deltaF = (double)(fs) / nLenData;
    //_ASSERT(deltaF == 1.0);
}
CRotateFreqAnsys::~CRotateFreqAnsys()
{
}
int CRotateFreqAnsys::getRotateFrequency()
{
	int i, j;
	if (pInputDatas->diagnosisParms.curRotaSpeed > 0)//curRotaSpeed初值为-1.0
	{
		*rotaFreq = pInputDatas->diagnosisParms.curRotaSpeed / 60.0;
		return 0;
	}

	double phaseRotaFreq[3];
	for (i = 0; i != 3; i++)
	{
		waveData = outputWaveDatas.waveData[i];
		curWaveData.assign(waveData.begin(), waveData.begin() + nLenData);
		nxtWaveData.assign(waveData.begin() + nLenData, waveData.end());

		phaseRotaFreq[i] = getOnePhaseRotaFreq();
	}
	*rotaFreq = phaseRotaFreq[0] + phaseRotaFreq[1] + phaseRotaFreq[2];

	return 0;
}
double CRotateFreqAnsys::getOnePhaseRotaFreq()
{
	if (getTwoWaveSpectrum())
	{
		return 1;
	}
	
	freq_power = getAccPowerBasicFreq();
	freq_harmonic = getAccHarmonicFreq();
	
	return getAccRotateFreq();
}
int CRotateFreqAnsys::getTwoWaveSpectrum()
{
	//CFFT_ANSYS *pCurFFT = new CFFT_ANSYS(curWaveData, windowType);
	//if (pCurFFT->FFTAnalysis())
	//{
	//	return 0;
	//}

	//int i;
	//SComplex tmp;
	//for (i = 0; i != curWaveData.size() / 2; i++)
	//{
	//	if (i == 0)
	//		curSpectrum.push_back(tmp);
	//	else
	//		curSpectrum.push_back(pCurFFT->vFFT_Ary[i]);
	//}
	//delete pCurFFT;
	//
	//CFFT_ANSYS *pNxtFFT = new CFFT_ANSYS(nxtWaveData, windowType);
	//if (pNxtFFT->FFTAnalysis())
	//{
	//	return 0;
	//}
	//for (i = 0; i != curWaveData.size() / 2; i++)
	//{
	//	if (i == 0)
	//		nxtSpectrum.push_back(tmp);
	//	else
	//		nxtSpectrum.push_back(pNxtFFT->vFFT_Ary[i]);
	//}
	//delete pNxtFFT;
	
	return 0;
}
double CRotateFreqAnsys::getAccPowerBasicFreq()
{
	int indexK;
	if (getPeakValueFromSpectrum(indexK))
	{
		return 0;
	}
	return getCurrentAccFreq(indexK);

	return 0;
}
double CRotateFreqAnsys::getCurrentAccFreq(int idx)
{
	double curAlpha = getPhaseAngle(curSpectrum[idx]);
	double nxtAlpha = getPhaseAngle(nxtSpectrum[idx]);	
	double deltaAlpha = curAlpha - nxtAlpha;
	
	shiftAngle(deltaAlpha);
	double deltaK = -deltaAlpha / TWO_PI;
	
	return (idx - deltaK)*deltaF;
}
int CRotateFreqAnsys::getPeakValueFromSpectrum(int &peakIdx)
{
	peakIdx = -1;
	double peakValue = MIN_DOU_VALUE;
	for (int i = 0; i != nLenData / 2; i++)
	{
		double tmpAmp = getAmplitude(curSpectrum[i]);
		if (peakValue < tmpAmp)
		{
			peakIdx = i;
			peakValue = tmpAmp;
		}
	}
	return 0;
}
double CRotateFreqAnsys::getAccHarmonicFreq()
{
	if (getRotaFreqByMotor_s())
	{
		return 1;
	}
	int harmonicIdx = searchHarmonicFreqIdx();

	freq_harmonic = getCurrentAccFreq(harmonicIdx);
	return 0;
}
int CRotateFreqAnsys::getRotaFreqByMotor_s()
{
	freq_range[0] = 2 * (1 - s_motor[1])*freq_power / p_motor;
	freq_range[1] = 2 * (1 - s_motor[0])*freq_power / p_motor;
	
	return 0;
}
int CRotateFreqAnsys::searchHarmonicFreqIdx()
{
	int iStr, iEnd;
	iStr = freq_range[0] / deltaF;
	iEnd = freq_range[1] / deltaF;

	int peakIdx = -1;
	double peakValue = MIN_DOU_VALUE;
	for (int i = iStr; i != iEnd; i++)
	{
		double tmpAmp = getAmplitude(curSpectrum[i]);
		if (peakValue < tmpAmp)
		{
			peakIdx = i;
			peakValue = tmpAmp;
		}
	}
	return 0;
}
double CRotateFreqAnsys::getAccRotateFreq()
{
	return (freq_harmonic - freq_power) / R_motor;
}
int CRotateFreqAnsys::shiftAngle(double &ang)
{
	if (ang < -ONE_PI)
		ang += TWO_PI;
	else if (ang > ONE_PI)
		ang -= TWO_PI;

    //_ASSERT(-ONE_PI < ang&&ang < ONE_PI);
	
	return 0;
}

