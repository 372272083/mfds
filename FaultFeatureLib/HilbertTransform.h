#include "Basic.hpp"
////////////////////////////////////////////////
struct SOutputFreqDmnDatas;
class CHilbert//
{
public:
	
	CHilbert(DOUBLE_VCT *pWaveData0);
	~CHilbert();

	int FFT_Num;
	DOUBLE_VCT *pWaveData;

public:
	int hilbertTransform_mthd_1();
			
	int hilbertTransform_mthd_2();

	int hartleyTransform();
	int reGetWaveEnvelope();
};
