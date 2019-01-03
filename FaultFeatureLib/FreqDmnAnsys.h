#include "Basic.hpp"
#include "IO_Param.h"
///////////////////////////////////////////////////////
class CFrequencyAnsys
{
public:	
	double bandWp, bandWs;

	int orderHarmonic;
	EnWindowType windowType;
	double modifyFactor;

	SOutputWaveDatas waveDatas;

	DOUBLE_VCT HarmProportion[6];
	SOutputFreqDmnDatas *pOutputFreqDmnDatas;

public:	
	CFrequencyAnsys(SOutputWaveDatas outputWaveDatas0,
					EnWindowType windowType0,
					SOutputFreqDmnDatas *pOutputTimeDmnDatas0);
	~CFrequencyAnsys();

	int frequencyDmnAnsys();
	int getWaveSpectrum();
	int getWaveEnvelope();
	int getSpectrumTrend();
	int getElcSpectrumTrend();
	int getVibSpectrumTrend();

	int getHarmonicComponent(double tolerance);
	int searchMaxFreq(int order,double basicFreq, double tolerance);
};