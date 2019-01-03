#include "Basic.hpp"

////////////////////////////////////////////////
class CRotateFreqAnsys//由电频计算转频
{
public:
	SDiagnosisInPutData *pInputDatas;
	SOutputWaveDatas outputWaveDatas;
	double *rotaFreq;

	EnWindowType windowType;
	
	int nLenData, fs;
	double deltaF;
	double s_motor[2];//电机转差率
	int R_motor;//电机槽数
	int p_motor;//电机极对数
	
	DOUBLE_VCT waveData, curWaveData, nxtWaveData;
	COMPLEX_VCT curSpectrum, nxtSpectrum;

	double freq_power;//电源基波频率
	double freq_range[2];//由转差率近似计算得到的谐波范围
	double freq_harmonic;//与转速相关的谐波频率
	DOUBLE_VCT rSpectrum, iSpectrum;

public:
	CRotateFreqAnsys(SDiagnosisInPutData *pInputDatas0,
		             SOutputWaveDatas waveDatas0,
					 double *rotaFreq0);
	
	~CRotateFreqAnsys();

	int getRotateFrequency();
	double getOnePhaseRotaFreq();
	int getTwoWaveSpectrum();
	
	double getAccPowerBasicFreq();
	double getAccHarmonicFreq();
	int searchHarmonicFreqIdx();

	double getCurrentAccFreq(int idx);
	double getAccRotateFreq();

	int getPeakValueFromSpectrum(int &peakIdx);
	
	int getRotaFreqByMotor_s();

	int shiftAngle(double &ang);
};
