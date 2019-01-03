#include "stdafx.h"
#include "MotorFaultDiagnosis.h"
#include <iostream>

#include "globals.h"
#include "IO_Param.h"
#include "DataPreProcess.h"
#include "TimeDmnAnsys.h"
#include "FreqDmnAnsys.h"
#include "RotateFreqAnsys.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CMotorFaultDiagnosis::CMotorFaultDiagnosis(SDiagnosisInPutData *pInputDatas0,
	                                       SDiagnosisOutPutData *pOutPutDatas0)
{
	askCurrentTime(str);

	pInputDatas = pInputDatas0;
	pOutPutDatas = pOutPutDatas0;
	
    //_ASSERT(pInputDatas->inputWaveDatas.nWaveData);
}
CMotorFaultDiagnosis::~CMotorFaultDiagnosis()
{
}
int CMotorFaultDiagnosis::motorFaultdiagnosing()
{
//[[数据预处理
	CDataPreProcess *pDataPreProcess;
	pDataPreProcess = new CDataPreProcess(pInputDatas,
	                                      &(pOutPutDatas->outputTimeDmnDatas));
	if (pDataPreProcess->inputDataPreProcessing())
	{
		return 1;
	}
	delete pDataPreProcess;
//]]
////////////////////////////////////////
//[[时域分析
	CTimeDmnAnsys *pTimeDomainAnsys;
	pTimeDomainAnsys = new CTimeDmnAnsys(&(pOutPutDatas->outputTimeDmnDatas));
	if (pTimeDomainAnsys->timeDomainAnsys())
	{
		return 1;
	}
	delete pTimeDomainAnsys;
//]]
////////////////////////////////////////
//[[转频分析
	//CRotateFreqAnsys *pRotateFreqAnsys;
	//pRotateFreqAnsys = new CRotateFreqAnsys(pInputDatas,
	//	                                    pOutPutDatas->outputTimeDmnDatas.outputWaveDatas,
	//										&(pOutPutDatas->outputFreqDmnDatas.rotaFreq));
	//if (pRotateFreqAnsys->getRotateFrequency())
	//{
	//	return 1;
	//}
	//delete pRotateFreqAnsys;
//]]
////////////////////////////////////////
//[[频域分析
	EnFreqAnsysType TYPE = TypeFFT;
	CFrequencyAnsys *pFrequencyAnsys;
	pFrequencyAnsys = new CFrequencyAnsys(pOutPutDatas->outputTimeDmnDatas.outputWaveDatas,
										  pInputDatas->diagnosisParms.systemParams.windowType,
										  &(pOutPutDatas->outputFreqDmnDatas));
	if (pFrequencyAnsys->frequencyDmnAnsys())
	{
		return 1;
	}
	delete pFrequencyAnsys;
//]]
////////////////////////////////////////
	askCurrentTime(end);
	double dTime1 = costTime(str, end);
	//std::cout << "使用时间:" << dTime1 << std::endl;
	//system("pause");

	return 0;
}





