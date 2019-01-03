#include "stdafx.h"
#include "globals.h"
#include "DFT_ANSYS.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
////////////////////////////////////////////////
CDFT_ANSYS::CDFT_ANSYS(DOUBLE_VCT vWaveData0, EnWindowType windowType0)
{
	QueryPerformanceCounter(&DFT_str);
	vWaveData = vWaveData0;
	windowType = windowType0;
	DFT_Num = vWaveData.size();
	
	double *windowCoeff = new double[DFT_Num];
	getWindowCoefficient(windowType, windowCoeff, DFT_Num);

	vDFT_Ary = new SComplex[DFT_Num];
	for (int i = 0; i < DFT_Num; i++)
	{
		vDFT_Ary[i].r = vWaveData[i] * windowCoeff[i];
	}
}
int CDFT_ANSYS::executeDFT()
{
	int i, j;
	SComplex *vTmpDFTAry = new SComplex[DFT_Num];
	for (i = 0; i != DFT_Num; i++) vTmpDFTAry[i] = vDFT_Ary[i];

	double tmp1, tmp2;
	tmp1 = TWO_PI / DFT_Num;
	for (i = 0; i != DFT_Num; i++)
	{
		SComplex curDFT, curWnk;
		for (j = 0; j != DFT_Num; j++)
		{
			tmp2 = tmp1*((i*j) % DFT_Num);
			curWnk.r = cos(tmp2);
			curWnk.i = -sin(tmp2);
			curDFT += vTmpDFTAry[j] * curWnk;
		}
		vDFT_Ary[i] = curDFT;
	}
	delete[]vTmpDFTAry;
	QueryPerformanceCounter(&DFT_end);
	dTime = costTime(DFT_str, DFT_end);
	
	return 0;
}
