#include "stdafx.h"
#include "globals.h"
#include "DataPreProcess.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CDataPreProcess::CDataPreProcess(SDiagnosisInPutData *inputDatas0,
				                 SOutputTimeDmnDatas *pOutputTimeDmnDatas0)
{
	pInputDatas = inputDatas0;
	pOutputTimeDmnDatas = pOutputTimeDmnDatas0;

	fs = pInputDatas->diagnosisParms.systemParams.fs;
	nOrder = pInputDatas->diagnosisParms.systemParams.nOrder;
}
CDataPreProcess::~CDataPreProcess(){}

int CDataPreProcess::inputDataPreProcessing()
{
	int i;
	DOUBLE_VCT curWave;

	for (i = 0; i != 3; i++)//电压
	{
		curWave = pInputDatas->inputWaveDatas.waveData[i];
		if (curWave.size() == 0) continue;

		//[[频域滤波--高通滤波或带通滤波
		if (ansysVolWs == -1)
		{
			hghPassFilter_TD(curWave, ansysVolWp);
		}
		else
		{
			bndPassFilter_TD(curWave, ansysVolWp, ansysVolWs);
		}//]]

		//[[去直流项
		if (dataHomogenization(curWave))
		{
			return 1;
		}//]]

		//[[去趋势项
		if (deleteTrending(curWave, pOutputTimeDmnDatas->outputWaveDatas.waveData[i]))
		{
			return 1;
		}//]]
	}
	///////////////////////////////////////////////////////////////////////////////////
	for (i = 3; i != 6; i++)//电流
	{
		curWave = pInputDatas->inputWaveDatas.waveData[i];
		if (curWave.size() == 0) continue;

		//[[频域滤波--高通滤波或带通滤波
		if (ansysCurWs == -1)
		{
			hghPassFilter_TD(curWave, ansysCurWp);
		}
		else
		{
			bndPassFilter_TD(curWave, ansysCurWp, ansysCurWs);
		}//]]

		//[[去直流项
		if (dataHomogenization(curWave))
		{
			return 1;
		}//]]

		//[[去趋势项
		if (deleteTrending(curWave, pOutputTimeDmnDatas->outputWaveDatas.waveData[i]))
		{
			return 1;
		}//]]
	}
	///////////////////////////////////////////////////////////////////////////////////
	for (i = 6; i != 12; i++)//加速度
	{
		curWave = pInputDatas->inputWaveDatas.waveData[i];
		if (curWave.size() == 0) continue;

		ansysAccWs = -1; ansysAccWp = 5;
		//[[频域滤波--高通滤波或带通滤波
		if (ansysAccWs == -1)
		{
			hghPassFilter_TD(curWave, ansysAccWp);
		}
		else
		{
			bndPassFilter_TD(curWave, ansysAccWp, ansysAccWs);
		}//]]

		//[[去直流项
		if (dataHomogenization(curWave))
		{
			return 1;
		}//]]

		//[[去趋势项
		if (deleteTrending(curWave, pOutputTimeDmnDatas->outputWaveDatas.waveData[i]))
		{
			return 1;
		}
	}

	//[[加速度积分得到速度
	for (i = 6; i != 12; i++)
	{
		if (pOutputTimeDmnDatas->outputWaveDatas.waveData[i].size() == 0) continue;
	
		curWave.clear();
		if (trapzIntg(pOutputTimeDmnDatas->outputWaveDatas.waveData[i], curWave))
		{
			return 1;
		}

		ansysSpdWs = -1; ansysSpdWp = 5;
		//[[频域滤波--高通滤波或带通滤波
		if (ansysSpdWs == -1)
		{
			hghPassFilter_TD(curWave, ansysSpdWp);
		}
		else
		{
			bndPassFilter_TD(curWave, ansysSpdWp, ansysSpdWs);
		}//]]

		//[[去直流项
		if (dataHomogenization(curWave))
		{
			return 1;
		}//]]

		//[[去趋势项
		if (deleteTrending(curWave, pOutputTimeDmnDatas->outputWaveDatas.waveData[i + 6]))
		{
			return 1;
		}
	}//]]

	//[[速度积分得到位移
	for (i = 12; i != 18; i++)
	{
		if (pOutputTimeDmnDatas->outputWaveDatas.waveData[i].size() == 0) continue;
		
		curWave.clear();
		if (trapzIntg(pOutputTimeDmnDatas->outputWaveDatas.waveData[i], curWave))
		{
			return 1;
		}

		ansysDisWs = -1; ansysDisWp = 5;
		//[[频域滤波--高通滤波或带通滤波
		if (ansysDisWs == -1)
		{
			hghPassFilter_TD(curWave,ansysDisWp);
		}
		else
		{
			bndPassFilter_TD(curWave,ansysDisWp, ansysDisWs);
		}//]]	
		
		if (dataHomogenization(curWave))
		{
			return 1;
		}

		if (deleteTrending(curWave, pOutputTimeDmnDatas->outputWaveDatas.waveData[i + 6]))
		{
			return 1;
		}
	}//]]


	for (i = 0; i != 24; i++)//数据长度检测
	{
		int curLen = pOutputTimeDmnDatas->outputWaveDatas.waveData[i].size();
        //_ASSERT(curLen == 0 || curLen == GLB_FS);
	}

	//FILE *stream;
	//DOUBLE_VCT wave;
	//stream = fopen("F:\\MFDS_OUTPUT\\0817\\Acc.txt", "w");
	//for (i = 0; i != 16384; i++)
	//{
	//	wave = pOutputTimeDmnDatas->outputWaveDatas.waveData[6];
	//	fprintf(stream, "%f\n", wave[i]);
	//}

	//stream = fopen("F:\\MFDS_OUTPUT\\0817\\Spd.txt", "w");
	//for (i = 0; i != 16384; i++)
	//{
	//	wave = pOutputTimeDmnDatas->outputWaveDatas.waveData[12];
	//	fprintf(stream, "%f\n", wave[i]);
	//}

	//stream = fopen("F:\\MFDS_OUTPUT\\0817\\Dis.txt", "w");
	//for (i = 0; i != 16384; i++)
	//{
	//	wave = pOutputTimeDmnDatas->outputWaveDatas.waveData[18];
	//	fprintf(stream, "%f\n", wave[i]);
	//}

	return 0;
}
int CDataPreProcess::waveDataPreProcessing(DOUBLE_VCT dataInti, DOUBLE_VCT &dataRslt)
{
	if (dataInti.size() == 0) return 0;
	
	DOUBLE_VCT tmpWave;
	if (dataHomogenization(dataInti))
	{
		return 1;
	}
	if (deleteTrending(tmpWave, dataRslt))
	{
		return 1;
	}
	return 0;
}
int CDataPreProcess::trapzIntg(DOUBLE_VCT dataInti,DOUBLE_VCT &dataRslt)
{
	int i, nDataSize;
	nDataSize = dataInti.size();
	if (nDataSize == 0) return 0;
	
	double intgDeltaT = 1000 / fs;
	dataRslt.push_back(0.0);
	for (i = 1; i != nDataSize; i++)
	{
		dataRslt.push_back((dataRslt[i - 1] + 0.5*(dataInti[i] + dataInti[i - 1])*intgDeltaT));
	}
	//dataRslt[0] = (dataRslt[1] + dataRslt[2]) / 2.0;
	
	return 0;
}

/*
int CDataPreProcess::dataHomogenization(DOUBLE_VCT &waveData)
{
#ifdef DELETE_DC
	int i, nDataSize;
	double avgValue = 0.0;
	nDataSize = waveData.size();
	for (i = 0; i != nDataSize; i++)
	{
		avgValue += waveData[i];
	}
	avgValue /= nDataSize;
	for (i = 0; i != nDataSize; i++)
	{
		waveData[i] -= avgValue;
	}
#endif

	return 0;
}
int CDataPreProcess::deleteTrending(DOUBLE_VCT dataInti,DOUBLE_VCT &dataRslt)
{
#ifdef TEST_TREND
	int i, j, k, nDataSize;
	double **vCoeffA, *vCoeffB, *result;
	vCoeffA = new double *[nOrder];
	vCoeffB = new double[nOrder];
	result = new double[nOrder];
	for (i = 0; i < nOrder; i++)
	{
		vCoeffB[i] = 0.0;
		vCoeffA[i] = new double[nOrder];
		for (j = 0; j < nOrder; j++)
			vCoeffA[i][j] = 0.0;
	}
	
	nDataSize = dataInti.size();
	double *vDvidedT = new double[nDataSize];
	for (i = 0; i < nDataSize; i++)
		vDvidedT[i] = i / fs;
	for (i = 0; i < nOrder; i++)
	{
		for (j = 0; j < nDataSize; j++)
		{
			vCoeffB[i] += dataInti[j] * pow(vDvidedT[j], i);
		}
	}

	for (i = 0; i < nOrder; i++)
	{
		for (j = 0; j < nOrder; j++)
		{
			for (k = 0; k < nDataSize; k++)
			{
				vCoeffA[i][j] += pow(vDvidedT[k], i + j);
			}
		}
	}	

//nOrder = 4;
//vCoeffA[0][0] = 2, vCoeffA[0][1] = 3, vCoeffA[0][2] = 4, vCoeffA[0][3] = -5;
//vCoeffA[1][0] = 6, vCoeffA[1][1] = 7, vCoeffA[1][2] = -8, vCoeffA[1][3] = 9;
//vCoeffA[2][0] = 10, vCoeffA[2][1] = 11, vCoeffA[2][2] = 12, vCoeffA[2][3] = 13;
//vCoeffA[3][0] = 14, vCoeffA[3][1] = 15, vCoeffA[3][2] = 16, vCoeffA[3][3] = 17;
//vCoeffB[0] = -6, vCoeffB[1] = 96, vCoeffB[2] = 312, vCoeffB[3] = 416;

	double *vTransfC = new double[nOrder];//存储初等行变换的系数，用于行的相减
	//消元的整个过程如下，总共nOrder-1次消元过程。
	for (i = 0; i < nOrder - 1; i++)
	{
		//求出第I次初等行变换的系数
		for (k = i + 1; k < nOrder; k++)
			vTransfC[k] = vCoeffA[k][i] / vCoeffA[i][i];

		//第I次的消元计算
		for (k = i + 1; k < nOrder; k++)
		{
			for (j = i; j < nOrder; j++)
			{
				vCoeffA[k][j] = vCoeffA[k][j] - vTransfC[k] * vCoeffA[i][j];
			}
			vCoeffB[k] = vCoeffB[k] - vTransfC[k] * vCoeffB[i];
		}
	}
	//解的存储数组
	//先计算出最后一个未知数；
	result[nOrder - 1] = vCoeffB[nOrder - 1] / vCoeffA[nOrder - 1][nOrder - 1];
	//求出每个未知数的值
	for (i = nOrder - 2; i >= 0; i--)
	{
		double sum = 0;
		for (j = i + 1; j < nOrder; j++)
		{
			sum += vCoeffA[i][j] * result[j];
		}
		result[i] = (vCoeffB[i] - sum) / vCoeffA[i][i];
	}
//////////////////////////////////////////////////////
	for (i = 0; i < nDataSize; i++)
	{
		double dTrend = result[0];
		for (j = 1; j < nOrder; j++)
		{
			dTrend += result[j] * pow(vDvidedT[i], j);
		}
		dataRslt.push_back(dataInti[i] - dTrend);
	}
//////////////////////////////////////////////////////
	for (i = 0; i < nOrder; i++)
	{
		delete[]vCoeffA[i]; 
		vCoeffA[i] = NULL;
	}
	delete[]vCoeffA; vCoeffA = NULL;
	delete[]vCoeffB; vCoeffB = NULL;
	delete[]vTransfC; vTransfC = NULL;
	delete[]vDvidedT; vDvidedT = NULL;
	delete[]result; result = NULL;

#endif

	return 0;
}
*/

int CDataPreProcess::inputDataSmoothing(DOUBLE_VCT dataInti, DOUBLE_VCT dataRslt)
{
	return 0;
}

