#include "stdafx.h"
#include "mfdspanel.h"
#include "IO_Param.h"

void motorFaultDiagnosis(SDiagnosisOutPutData *pOutPutDatas)
{

}

void transformWaveDatas(SDiagnosisInPutData *pInPutDatas)
{
	int i, j;

	if (pInPutDatas->inputWaveDatas.waveData[0].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[0][i] =
				(pInPutDatas->inputWaveDatas.waveData[0][i] - pInPutDatas->featDatas.elcFactor[0])* pInPutDatas->featDatas.elcFactor[12] / pInPutDatas->featDatas.elcFactor[6];
		}
	}

	if (pInPutDatas->inputWaveDatas.waveData[1].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[1][i] =
				(pInPutDatas->inputWaveDatas.waveData[1][i] - pInPutDatas->featDatas.elcFactor[1])* pInPutDatas->featDatas.elcFactor[13] / pInPutDatas->featDatas.elcFactor[7];
		}
	}

	if (pInPutDatas->inputWaveDatas.waveData[2].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[2][i] =
				(pInPutDatas->inputWaveDatas.waveData[2][i] - pInPutDatas->featDatas.elcFactor[2])* pInPutDatas->featDatas.elcFactor[14] / pInPutDatas->featDatas.elcFactor[8];
		}
	}

	if (pInPutDatas->inputWaveDatas.waveData[3].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[3][i] =
				(pInPutDatas->inputWaveDatas.waveData[3][i] - pInPutDatas->featDatas.elcFactor[3])* pInPutDatas->featDatas.elcFactor[15] / pInPutDatas->featDatas.elcFactor[9];
		}
	}

	if (pInPutDatas->inputWaveDatas.waveData[4].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[4][i] =
				(pInPutDatas->inputWaveDatas.waveData[4][i] - pInPutDatas->featDatas.elcFactor[4])* pInPutDatas->featDatas.elcFactor[16] / pInPutDatas->featDatas.elcFactor[10];
		}
	}

	if (pInPutDatas->inputWaveDatas.waveData[5].size() != 0)
	{
		for (i = 0; i != 16384; i++)
		{
			pInPutDatas->inputWaveDatas.waveData[5][i] =
				(pInPutDatas->inputWaveDatas.waveData[5][i] - pInPutDatas->featDatas.elcFactor[5])* pInPutDatas->featDatas.elcFactor[17] / pInPutDatas->featDatas.elcFactor[11];
		}
	}

	for (i = 6; i != 12; i++)
	{
		for (j = 0; j != 16384; j++)
		{
			if (pInPutDatas->inputWaveDatas.waveData[i].size() != 0)
			{
				pInPutDatas->inputWaveDatas.waveData[i][j] =
					(pInPutDatas->inputWaveDatas.waveData[i][j] - 5000) / 10;
			}
		}
	}
	//for (i = 0; i != 12; i++)
	//{
	//	if (pInPutDatas->inputWaveDatas.waveData[i].size() != 0)
	//	{
	//		pInPutDatas->inputWaveDatas.waveData[i][0] =
	//			pInPutDatas->inputWaveDatas.waveData[i][1];
	//	}
	//}
}

void mfds(SDiagnosisInPutData *pInputDatas, SDiagnosisOutPutData *pOutPutDatas)
{
	transformWaveDatas(pInputDatas);

	CMotorFaultDiagnosis *pCMotorFaultDiagnosis;
	pCMotorFaultDiagnosis = new CMotorFaultDiagnosis(pInputDatas, pOutPutDatas);
	pCMotorFaultDiagnosis->motorFaultdiagnosing();
	delete pCMotorFaultDiagnosis;

	////将pOutPutDatas写入数据库
	//fdb.SaveOutputData(pOutPutDatas);
	//if (pInputDatas->systemMode == diagnosisMode)
	//{
	//	motorFaultDiagnosis(pOutPutDatas);
	//}

	//stream = fopen("F:\\MFDS_OUTPUT\\Freq.txt", "w");
	//nSize=pOutPutDatas->outputFreqDmnDatas.amptFreq[0].size();
	//for (int i = 0; i != nSize; i++)
	//{
	//	double tmp = pOutPutDatas->outputFreqDmnDatas.amptFreq[0][i];
	//	fprintf(stream, "%f\n", tmp);
	//}
	//return 0;/**/
}