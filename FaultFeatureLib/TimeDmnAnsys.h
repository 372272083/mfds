#include "Basic.hpp"
#include "IO_Param.h"
/////////////////////////////////////////////
struct SGuoBiao
{
//[[ GB 10068 - 2008
	//H--轴中心高
	//f--free自由悬置；r--rigid刚性安装
	//A--振动等级A适用于对振动无特殊要求的电机
	//B--振动等级B适用于对振动有特殊要求的电机，此时H<= 123时，不考虑刚性安装
	
	///振动等级A	
	//56 <= H <= 123
	/*
	double GB_A_f_s1 = 25, GB_A_f_v1 = 1.6, GB_A_f_a1 = 2.5;
	double GB_A_r_s1 = 21, GB_A_r_v1 = 1.3, GB_A_r_a1 = 2.0;
	//132 < H <= 280
	double GB_A_f_s2 = 35, GB_A_f_v2 = 2.2, GB_A_f_a2 = 3.5;
	double GB_A_r_s2 = 29, GB_A_r_v2 = 1.8, GB_A_r_a2 = 2.8;
		//280 < H
	double GB_A_f_s3 = 45, GB_A_f_v3 = 2.8, GB_A_f_a3 = 4.4;
	double GB_A_r_s3 = 37, GB_A_r_v3 = 2.3, GB_A_r_a3 = 3.6;
	
	///振动等级B	
	//56 <= H <= 123
	double GB_B_f_s1 = 11, GB_B_f_v1 = 0.7, GB_B_f_a1 = 1.1;

	//132 < H <= 280
	double GB_B_f_s2 = 18, GB_B_f_v2 = 1.1, GB_B_f_a2 = 1.7;
	double GB_B_r_s2 = 14, GB_B_r_v2 = 0.9, GB_B_r_a2 = 1.4;

	//280 < H
	double GB_B_f_s3 = 29, GB_B_f_v3 = 1.8, GB_B_f_a3 = 2.8;
	double GB_B_r_s3 = 24, GB_B_r_v3 = 1.5, GB_B_r_a3 = 2.4;
	*/

	//滑动轴承且转速>1200;P>1000Kw时，需要检测轴相对振动和径向跳动
//GB 10068 - 2008 ]]
};
/////////////////////////////////////////////
class CTimeDmnAnsys
{
public:
	double fs, motorPower, centerHigh;

	double maxValue[24], minValue[24], peakVueValue[24];
	double aveValue[24], variance[24], RMS_value[24], kurtosis[24];

	double waveIndex[24];//波形指标
	
	double peakVueIndex[24];//峰值指标
	/*峰值指标反映了峰值的变化程度。过大的峰值指标通常对应着局部缺陷，一般正常时，
	峰值指标约在 3～5 之间，有故障时峰值指标增大，一般当峰值指标大于 5 时有局部缺陷*/

	double pulseIndex[24];//脉冲指标
	double KurtosisIndex[24];//峭度指标
	
	int nWaveData;
	SOutputTimeDmnDatas *pOutputTimeDmnDatas;

public:
	CTimeDmnAnsys(SOutputTimeDmnDatas *pOutputFeatDatas);
	~CTimeDmnAnsys();

	int timeDomainAnsys();
	
	void initialize();
	int getTimeDmnFeaVaule();
	int getAxisLocus();

	int getWaveFactor();
	int getPeakVueFactor();
	int getKurtosisCoeffi();
	int getAmplitudeDensity();
};