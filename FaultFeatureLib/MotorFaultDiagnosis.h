#include "Basic.hpp"

struct SDiagnosisInPutData;
struct SDiagnosisOutPutData;
////////////////////////////////////////////////
class CMotorFaultDiagnosis
{
public:
	int nWaveData;
	SDiagnosisInPutData *pInputDatas;
	SDiagnosisOutPutData *pOutPutDatas;

	LARGE_INTEGER str, end;

	CMotorFaultDiagnosis(SDiagnosisInPutData *pInputDatas0,
	  	                 SDiagnosisOutPutData *pOutPutDatas0);
	~CMotorFaultDiagnosis();

public:
	int motorFaultdiagnosing();
};
