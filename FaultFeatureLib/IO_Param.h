#pragma once

#include "Basic.hpp"
////////////////////////////////////////////////
struct SInputFeatDatas
{
	double elcFactor[18];
	double elcFeatDatas[27];
	double vibFeatDatas[18];
	double tmpFeatDatas[12];

	SInputFeatDatas()
	{
		elcFactor[0] = 32866.3359;
		elcFactor[1] = 32699.6348;
		elcFactor[2] = 32863.4805;
		elcFactor[3] = 32772.6250;
		elcFactor[4] = 32700.7441;
		elcFactor[5] = 32819.6563;
		elcFactor[6] = 215.1515;
		elcFactor[7] = 215.4452;
		elcFactor[8] = 215.6028;
		elcFactor[9] = 4324.1147;
		elcFactor[10] = 4319.8555;
		elcFactor[11] = 4313.9473;

		elcFactor[12] = 1.0;
		elcFactor[13] = 1.0;
		elcFactor[14] = 1.0;
		elcFactor[15] = 1.0;
		elcFactor[16] = 1.0;
		elcFactor[17] = 1.0;
	}
};
struct SInputWaveDatas
{
	int nWaveData;
	DOUBLE_VCT waveData[12];
	//波形数据依次为
	//0-waveVolA,  1-waveVolB,  2-waveVolC;
	//3-waveCurA,  4-waveCurB,  5-waveCurC;
	//6-waveVibX1, 7-waveVibY1, 8-waveVibZ1;
	//9-waveVibX2, 10-waveVibY2, 11-waveVibZ2;
	SInputWaveDatas() { nWaveData = 12; }
};
struct SOutputWaveDatas
{
	int nWaveData;
	DOUBLE_VCT waveData[24];
	//波形数据
	//0-waveVolA,  1-waveVolB,  2-waveVolC;
	//3-waveCurA,  4-waveCurB,  5-waveCurC;
	//6-waveVibX1, 7-waveVibY1, 8-waveVibZ1;
	//9-waveVibX2, 10-waveVibY2, 11-waveVibZ2;

	//积分数据
	//12-waveSpdX1, 13-waveSpdY1, 14-waveSpdZ1;
	//15-waveSpdX2, 16-waveSpdY2, 17-waveSpdZ2;
	//18-waveDisX1, 19-waveDisY1, 20-waveDisZ1;
	//21-waveDisX2, 22-waveDisY2, 23-waveDisZ2;
	SOutputWaveDatas() { nWaveData = 24; }
};
//////////////////////////////////////////////
struct SDiagnosisThreshold
{
	double tmpAmpThreshold[12];

	double AmpThreshold[24];
	double pkpkThreshold[24];
	double RMS_Threshold[24];

	double waveThreshold[24];
	double ppThreshold[24];
	double pulseThreshold[24];
	double kurtosisThreshold[24];
	double marginThreshold[24];
	double skewnessThreshold[24];
	
	SDiagnosisThreshold()
	{
		int i;
		for (i = 0; i != 12; i++)
			tmpAmpThreshold[i] = -1.0;
		for (i = 0; i != 24; i++)
		{
			AmpThreshold[24] = -1.0;
			pkpkThreshold[24] = -1.0;
			RMS_Threshold[24] = -1.0;

			waveThreshold[i] = -1.0;
			ppThreshold[i] = -1.0;
			pulseThreshold[i] = -1.0;
			kurtosisThreshold[i] = -1.0;
			marginThreshold[i] = -1.0;
			skewnessThreshold[i] = -1.0;
		}
	}
};
struct SSystemParams
{
	double fs;
	int nOrder;
	EnWindowType windowType;//窗口类型
	SSystemParams()
	{
		fs = 8192;
		nOrder = 4;
		windowType = HanningWindow;
	}
};
struct SFilterParams
{
	double ansysCurWp, ansysCurWs;
	double ansysVolWp, ansysVolWs;
	double ansysAccWp, ansysAccWs;
	double ansysSpdWp, ansysSpdWs;

	double showCurWp, showCurWs;
	double showVolWp, showVolWs;
	double showAccWp, showAccWs;
	double showSpdWp, showSpdWs;
	SFilterParams()
	{
		ansysCurWp = ansysCurWs = -1;
		ansysVolWp = ansysVolWs = -1;
		ansysAccWp = ansysAccWs = -1;
		ansysSpdWp = ansysSpdWs = -1;

		showCurWp = showCurWs = -1;
		showVolWp = showVolWs = -1;
		showAccWp = showAccWs = -1;
		showSpdWp = showSpdWs = -1;

	}
};
struct SLoadParams
{
	EnLoadType loadType;
	EnRunCondition runCondition;
};
struct SInputDiagnosisParms
{
	double curRotaSpeed;//当前转速

	EnWiringType wiringType;//接线方式
	SLoadParams loadParams;//载荷参数
	SSystemParams systemParams;//系统参数
	SInputDiagnosisParms()
	{
		curRotaSpeed = -1.0;
	}
};
struct SNameplateData
{
	EnMotorType motorType;//电机类型
	double motorPower;//电机功率
	double ratedVoltage;//额定电压
	double ratedCurrent;//额定电流
	double ratedSpeed[2];//额定转速---值或范围
	int polePairs;//电机极对数
	int motorSlot;//电机槽数
	EnInsulationLevel insulationLevel;//绝缘等级
	double powerFactor;//功率因素
	double centerHigh;//电机中心高
	SNameplateData()
	{
		motorPower = -1.0;
		ratedVoltage = -1.0;
		ratedCurrent = -1.0;
		ratedSpeed[0] = -1.0;
		ratedSpeed[1] = -1.0;;
		polePairs = -1;
		motorSlot = -1;
		powerFactor = -1.0;
		centerHigh = -1.0;
	}
};
struct SInputBearLib_ID
{
	EnBearType bearType;//滚动轴承-滑动轴承
	EnBearModel bearModel; //轴承型号
	double dBearRoller;//滚动体直径
	double dBearRetainer;//保持架直径
	double angleAlphe;//滚动体接触角
};
struct SMotorLib_ID
{
	SNameplateData nameplateData;
	SInputBearLib_ID bearParam;
};
/////////////////////////////////////////////////
struct SDiagnosisInPutData
{
	EnSystemMode systemMode;
	SInputDiagnosisParms diagnosisParms;
	SInputFeatDatas featDatas;
	SInputWaveDatas inputWaveDatas;
	SDiagnosisThreshold diaThreshold;
	SMotorLib_ID motorLib_ID;

	~SDiagnosisInPutData(){}
};
/////////////////////////////////////////////////
struct SOutputTimeDmnDatas
{
	double elcFeatDatas[27];//与输入的27个电量特征数据对应

	double vTmpAmp[12];

	double vWaveAmp[24];
	double vWaveRMS[24];
	double vWavePkPkValue[24]; 

	double waveIndex[24];
	double peakVueIndex[24];
	double pulseIndex[24];
	double kurtosisIndex[24];
	
	DOUBLE_VCT axisLocusVct_1[4]; //第一类轴心轨迹图

	DOUBLE_VCT axisLocusVct_2[2]; //第二类轴心轨迹图

	SOutputWaveDatas outputWaveDatas;
	SOutputTimeDmnDatas()
	{
		int i;
		for (i = 0; i != 27; i++)
			elcFeatDatas[i] = -1.0;
		
		for (i = 0; i != 12; i++)
			vTmpAmp[i] = -1.0;
		
		for (i = 0; i != 24; i++)
		{
			vWaveAmp[24] = -1.0;
			vWaveRMS[24] = -1.0;
			vWavePkPkValue[24] = -1.0;

			waveIndex[i] = -1.0;
			peakVueIndex[i] = -1.0;
			pulseIndex[i] = -1.0;
			kurtosisIndex[i] = -1.0;
		}
	}
};
struct SOutputFreqDmnDatas
{
	double deltaF;
	double rotaFreq;

	DOUBLE_VCT amptFreq[18];//幅值谱
	DOUBLE_VCT phasFreq[18];//相位谱
	
	DOUBLE_VCT iPowerSpectrum[18];//功率谱
	DOUBLE_VCT rPowerSpectrum[18];
	
	DOUBLE_VCT amptEnvelopeFreq[6];//包络谱
	DOUBLE_VCT phasEnvelopeFreq[6];

	DOUBLE_VCT HarmProportion[6];

	SOutputFreqDmnDatas()
	{
		deltaF = -1.0;
		rotaFreq = -1.0;
	}
};

struct SDiagnosisOutPutData
{
	int faultType[36];
	int faultProbability[36];
	int faultMessage[36];

	SOutputTimeDmnDatas outputTimeDmnDatas;
	SOutputFreqDmnDatas outputFreqDmnDatas;
	SDiagnosisOutPutData()
	{
		for (int i = 0; i != 24; i++)
		{
			faultType[i] = -1;
			faultProbability[i] = -1;
			faultMessage[i] = -1;
		}
	}
	~SDiagnosisOutPutData(){};
};
struct SInteractParams//交互参数
{
	//double 
};