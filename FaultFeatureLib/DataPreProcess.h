#include "Basic.hpp"
#include "IO_Param.h"

////////////////////////////////////////////////
class CDataPreProcess//数据前处理
{
public:
	int nOrder;
	double fs;

	double ansysVolWp, ansysVolWs;
	double ansysCurWp, ansysCurWs;
	double ansysAccWp, ansysAccWs;
	double ansysSpdWp, ansysSpdWs;
	double ansysDisWp, ansysDisWs;

	SDiagnosisInPutData *pInputDatas;
	SOutputTimeDmnDatas *pOutputTimeDmnDatas;
public:
	CDataPreProcess(SDiagnosisInPutData *pInputDatas0,
				    SOutputTimeDmnDatas *pOutputTimeDmnDatas0);
	~CDataPreProcess();

	int trapzIntg(DOUBLE_VCT dataInti,DOUBLE_VCT &dataRslt);//

	//int dataHomogenization(DOUBLE_VCT &waveData);//零均质化处理
	//int deleteTrending(DOUBLE_VCT dataInti, DOUBLE_VCT &dataRslt);
	
	int inputDataPreProcessing();
	int waveDataPreProcessing(DOUBLE_VCT dataInti,DOUBLE_VCT &dataRslt);
	
	int inputDataSmoothing(DOUBLE_VCT dataInti,DOUBLE_VCT dataRslt);
};
