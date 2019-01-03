#include "Basic.hpp"
//#include "IO_Param.h"
///////////////////////////////////////////////////////
class CFFT_ANSYS
{
public:	
	double *vWaveAry;
	DOUBLE_VCT vWaveVct;
	DOUBLE_VCT *pWaveVct;
	EnWindowType windowType;
	int FFT_Layer, FFT_Num;
	
	double freq1, freq2;

	LARGE_INTEGER FFT_str, FFT_end;
	double dTime;
public:
	~CFFT_ANSYS();
	
	CFFT_ANSYS(DOUBLE_VCT vWaveVct0,
		       EnWindowType windowType0);	
	
	CFFT_ANSYS(double vWaveAry[],
				int FFT_Num0,
				double fp1,
				double fp2);

	CFFT_ANSYS(DOUBLE_VCT *pWaveVct0,
				double fp1,
				double fp2);

	void initialize();
	int FFTAnalysis();
	int IFFTAnalysis();
	
	int lowPassByFFT_TD();
	int hghPassByFFT_TD();
	int bndPassByFFT_TD();
	int bndStopByFFT_TD();
};