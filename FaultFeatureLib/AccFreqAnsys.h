#include "Basic.hpp"
////////////////////////////////////////////////
class CAccFreqAnsys//精确计算频谱
{
public:
	CAccFreqAnsys(DOUBLE_VCT waveData0, 
		          int fs0,
				  EnWindowType windowType0,
			      EnFreqAnsysType ansysType0);
	~CAccFreqAnsys();

	int fs;
	DOUBLE_VCT waveData;
	EnWindowType windowType;
	EnFreqAnsysType ansysType;

public:
	int getAccFrequency();
};
