#include "stdafx.h"
#include "globals.h"
#include "DFT_ANSYS.h"
#include "FFT_ANSYS.h"
#include "HilbertTransform.h"
#include "FreqDmnAnsys.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 

////////////////////////////////////////////////
CFrequencyAnsys::CFrequencyAnsys(SOutputWaveDatas waveDatas0,
								 EnWindowType windowType0,
								 SOutputFreqDmnDatas *pOutputFreqDmnDatas0)
{
	waveDatas = waveDatas0;
	windowType = windowType0;
	pOutputFreqDmnDatas = pOutputFreqDmnDatas0;
windowType = RectangleWindow;
	switch (windowType)
	{
		case RectangleWindow:
		{
			modifyFactor = 1; break;
		}
		case HanningWindow:
		{
			modifyFactor = 2; break;
		}
		case HammingWindow:
		{
			modifyFactor = 1.852; break;
		}
		default:
		{
			windowType = HanningWindow;
			modifyFactor = 2;
		}
	}
}
CFrequencyAnsys::~CFrequencyAnsys()
{

}
int CFrequencyAnsys::frequencyDmnAnsys()
{
	if (getWaveSpectrum())
	{
		return 1;
	}
	if (getWaveEnvelope())
	{
		return 1;
	}
	if (getSpectrumTrend())
	{
		return 1;
	}

	return 0;
}
int CFrequencyAnsys::getWaveSpectrum()
{
	int i, j;
	for (i = 0; i < 18; i++)
	{
		int lenWaveData = waveDatas.waveData[i].size();
		if (lenWaveData == 0) continue;

		CFFT_ANSYS *pFFT_ANSYS = new CFFT_ANSYS(waveDatas.waveData[i],
												windowType);//windowType//RectangleWindow//
		pFFT_ANSYS->FFTAnalysis();
		double tmpCoff = modifyFactor / lenWaveData;
		for (j = 0; j != lenWaveData / 2 + 1; j++)
		{
			double tmpAmpt, tmpPhas;
			if (j == 0)
				tmpAmpt = getAmplitude(Glb_Cmp_Ary[j])*tmpCoff;
			else
				tmpAmpt = 2 * getAmplitude(Glb_Cmp_Ary[j])*tmpCoff;
			tmpPhas = getPhaseAngle(Glb_Cmp_Ary[j]);

			pOutputFreqDmnDatas->amptFreq[i].push_back(tmpAmpt);
			pOutputFreqDmnDatas->phasFreq[i].push_back(tmpPhas);
		}
		clearGlbAry();

		delete pFFT_ANSYS;
	}
	//FILE *stream;
	//DOUBLE_VCT wave;
	//stream = fopen("F:\\MFDS_OUTPUT\\0817\\AccFreq.txt", "w");
	//for (i = 0; i != 8192; i++)
	//{
	//	wave = pOutputFreqDmnDatas->amptFreq[6];
	//	fprintf(stream, "%f\n", wave[i]);
	//}

	return 0;
}
int CFrequencyAnsys::getSpectrumTrend()
{
	int i, j;
	for (i = 0; i < 6; i++)
	{
		//getElcSpectrumTrend()->amptFreq[i]
	}
	for (i = 12; i < 18; i++)
	{
		//getVibSpectrumTrend()->amptFreq[i]
	}
	return 0;
}
int CFrequencyAnsys::getElcSpectrumTrend()
{
	return 0;
}
int CFrequencyAnsys::getVibSpectrumTrend()
{
	return 0;
}
int CFrequencyAnsys::getHarmonicComponent(double tolerance)
{
	int i, j;
	for (i = 0; i < 6; i++)
	{
		double basicFreq;
		HarmProportion[i].push_back(basicFreq);
		for (j = 1; j < orderHarmonic; j++)
		{
			double dHarmonic = searchMaxFreq(j, basicFreq, tolerance);
			HarmProportion[i].push_back(dHarmonic / basicFreq);
		}
	}
	return 0;
}
int CFrequencyAnsys::searchMaxFreq(int order, double basicFreq, double tolerance)
{
	//int i;
	//double maxFreq = MIN_DOU_VALUE;
	//int iStr = 2 * (basicFreq*order - tolerance);
	//int iEnd = 2 * (basicFreq*order + tolerance);
	//for (i = iStr; i < iEnd; i++)
	//{
	//	double iTmp;
	//	double rTmp;
	//	if (maxFreq < getAmplitude(iTmp, rTmp))
	//	{
	//		maxFreq = getAmplitude(iTmp, rTmp);
	//	}
	//}
	return 0;
}
int CFrequencyAnsys::getWaveEnvelope()
{
	int i, j;
	for (i = 6; i < 12; i++)
	{
		int lenWaveData = waveDatas.waveData[i].size();
		if (lenWaveData == 0) continue;
		
		DOUBLE_VCT filterWave(lenWaveData);
		DOUBLE_VCT hilbrtWave(lenWaveData);

		filterWave = waveDatas.waveData[i];

		bandWp = 3000;bandWs = 4000;
		bndPassFilter_TD(filterWave, bandWp, bandWs);

		hilbrtWave = filterWave;
		CHilbert *pCHilbert1 = new CHilbert(&hilbrtWave);
		pCHilbert1->hilbertTransform_mthd_1();
		delete pCHilbert1;
		
		//计算包络线
		DOUBLE_VCT vEnveWave;
		for (j = 0; j < lenWaveData; j++)
		{
			double tmp = sqrt(filterWave[j] * filterWave[j] + hilbrtWave[j] * hilbrtWave[j]);
			vEnveWave.push_back(tmp);
		}
	
		CFFT_ANSYS *pFFT_ANSYS = new CFFT_ANSYS(vEnveWave,
												windowType);//HanningWindow////RectangleWindow
		pFFT_ANSYS->FFTAnalysis();
		for (j = 0; j != lenWaveData / 2; j++) 
		{
			double tmpAmpt, tmpPhas;
			if (j == 0)
				tmpAmpt = getAmplitude(Glb_Cmp_Ary[j]) / lenWaveData;
			else
				tmpAmpt = 2 * getAmplitude(Glb_Cmp_Ary[j]) / lenWaveData;
			tmpPhas = getPhaseAngle(Glb_Cmp_Ary[j]);
			pOutputFreqDmnDatas->amptEnvelopeFreq[i-6].push_back(tmpAmpt);
			pOutputFreqDmnDatas->phasEnvelopeFreq[i-6].push_back(tmpPhas);
		}
		clearGlbAry();
	}

	//FILE *stream;

	//DOUBLE_VCT wave;
	//stream = fopen("F:\\MFDS_OUTPUT\\0817\\EnvelopeFreq.txt", "w");
	//for (int j = 0; j != 8192; j++)
	//{
	//	wave = pOutputFreqDmnDatas->amptEnvelopeFreq[0];
	//	fprintf(stream, "%f\n", wave[j]);
	//}

	return 0;
}