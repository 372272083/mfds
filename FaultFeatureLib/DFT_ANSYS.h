#include "Basic.hpp"
#include "IO_Param.h"
///////////////////////////////////////////////////////
class CDFT_ANSYS
{
public:
	int DFT_Num;
	DOUBLE_VCT vWaveDat;
	EnWindowType windowType;
	SComplex *vW_nk,*vDFT_Ary;

	double dTime;
	DOUBLE_VCT vWaveData;
	LARGE_INTEGER DFT_str, DFT_end;

public:
	CDFT_ANSYS(DOUBLE_VCT vWaveData0, EnWindowType windowType0);
	
	int executeDFT();
};
