#include "stdafx.h"
#include "DFT_ANSYS.h"
#include "FFT_ANSYS.h"
#include "AccFreqAnsys.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CAccFreqAnsys::CAccFreqAnsys(DOUBLE_VCT waveData0, 
	                           int fs0,
							   EnWindowType windowType0,
							   EnFreqAnsysType ansysType0)
{
	waveData = waveData0;
	fs = fs0;
	windowType = windowType0;
	ansysType = ansysType0;
}
CAccFreqAnsys::~CAccFreqAnsys()
{
}
int CAccFreqAnsys::getAccFrequency()
{
	//COMPLEX_VCT spectrum;
	//if (ansysType == TypeFFT)
	//{
	//	CFFT_ANSYS *pCFFT_ANSYS = new CFFT_ANSYS(waveData, windowType);
	//	if (pCFFT_ANSYS->FFTAnalysis())
	//	{
	//		return 0;
	//	}
	//}
	//else
	//{
	//	CDFT_ANSYS *pCDFT_ANSYS = new CDFT_ANSYS(waveData, windowType);
	//}
	return 0;
}




