#include "stdafx.h"
#include "globals.h"
#include "FFT_ANSYS.h"
#include "Basic.hpp"

//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 

SComplex Glb_Cmp_Ary[GLB_FS];


void clearGlbAry()
{
	for (int i = 0; i != GLB_FS; i++)
	{
		Glb_Cmp_Ary[i].i = 0.0;
		Glb_Cmp_Ary[i].r = 0.0;
	}
}

void askCurrentTime(LARGE_INTEGER& curTime)
{
	QueryPerformanceCounter(&curTime);
}
double costTime(LARGE_INTEGER str, LARGE_INTEGER end)
{
	double dTime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	end.QuadPart -= str.QuadPart;
	end.QuadPart *= 1000;
	end.QuadPart /= freq.QuadPart;

	if (end.HighPart != 0) return -1;
	else dTime = end.LowPart;
	
	return dTime;
}
//////////////////////////////////////////////////
double getPhaseAngle(SComplex cmp)
{
	return atan2(cmp.i ,cmp.r);
}
double getAmplitude(SComplex cmp)
{
	return sqrt(cmp.i*cmp.i + cmp.r*cmp.r);
}
double getAmplitude(double image, double real)
{
	return sqrt(image*image + real*real);
}
void getRectanlCoefficients(double *vCoeff, int num)
{
	for (int i = 0; i != num; i++)
		vCoeff[i] = 1.0;
}
void getHanningCoefficients(double *vCoeff, int num)
{
	vCoeff[0] = 0.0;
	for (int i = 1; i != num / 2 + 1; i++)
	{
		vCoeff[i] = 0.5 - 0.5*cos(TWO_PI*i / num);
		vCoeff[num - i] = vCoeff[i];
	}
}
void getHammingCoefficients(double *vCoeff, int num)
{
	for (int i = 0; i != (num + 1) / 2; i++)
	{
		vCoeff[i] = 0.54 - 0.46*cos(TWO_PI*i / (num - 1));
		vCoeff[num - 1 - i] = vCoeff[i];
	}
}
int getWindowCoefficient(EnWindowType windowType, double *coeff, int num)
{
	switch (windowType)
	{
	case RectangleWindow:
		{
			getRectanlCoefficients(coeff, num);
		}
		break;
	case HanningWindow:
		{
			getHanningCoefficients(coeff, num);
		}
		break;
	case HammingWindow:
		{
			getHammingCoefficients(coeff, num);
		}
		break;
	}
	return 0;
}
void CLARKE_Transform(double *value_3Phase, double *value_2Phase)
{
	double arry_T[3][2] = { 1,    -0.5,        -0.5, 
		                    0, HALF_SQRT_3, -HALF_SQRT_3 };
}
void PARK_Transform(double *value_2Phase_S, double theta, double *value_2Phase_R)
{
	double cosTheta = cos(theta);
	double sinTheta = sin(theta);
	double arry_T[2][2] = {cosTheta,  sinTheta,
		                   -cosTheta, sinTheta};
}
//////////////**************************//////////////
int lowPassFilter_TD(double wave[],int num,double fp)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(wave, num, fp, -1.0);
	pFilter->lowPassByFFT_TD();
	delete pFilter;

	return 0;
}
int hghPassFilter_TD(double wave[], int num, double fp)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(wave, num, fp, -1.0);
	pFilter->hghPassByFFT_TD();
	delete pFilter;

	return 0;
}
int bndPassFilter_TD(double wave[], int num, double fp1, double fp2)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(wave, num, fp1, fp2);
	pFilter->bndPassByFFT_TD();
	delete pFilter;
	
	return 0;
}
int bndStopFilter_TD(double wave[], int num, double fp1, double fp2)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(wave, num, fp1, fp2);
	pFilter->bndStopByFFT_TD();
	delete pFilter;
	
	return 0;
}

int lowPassFilter_FD(SComplex freq[], int num, double fp)
{
	return 0;
}
int hghPassFilter_FD(SComplex freq[], int num, double fp)
{
	int iEnd = ceil(2 * fp);
	for (int i = 0; i != iEnd; i++)
	{
		freq[i].r = 0.0;
		freq[i].i = 0.0;
	}
	return 0;
}
int bndPassFilter_FD(SComplex freq[], int num, double fp1, double fp2)
{
	int iStr = ceil(2 * fp1);
	int iEnd = floor(2 * fp2);
	for (int i = 0; i != num; i++)
	{
		if (i<iStr || i>iEnd)
		{
			freq[i].r = 0.0;
			freq[i].i = 0.0;
		}
	}
	return 0;
}
int bndStopFilter_FD(SComplex freq[], int num, double fp1, double fp2)
{
	int iStr = 2 * fp1;
	int iEnd = 2 * fp2;
	for (int i = iStr; i != iEnd; i++)
	{
		freq[i].r = 0.0;
		freq[i].i = 0.0;
	}
	return 0;
}

void lowPassFilter_TD(DOUBLE_VCT &wave, double fp)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(&wave, fp, -1.0);
	pFilter->hghPassByFFT_TD();
	clearGlbAry();

	delete pFilter;
}
void hghPassFilter_TD(DOUBLE_VCT &wave, double fp)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(&wave, fp, -1.0);
	pFilter->hghPassByFFT_TD();
	clearGlbAry();

	delete pFilter;
}
void bndPassFilter_TD(DOUBLE_VCT &wave, double fp1, double fp2)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(&wave, fp1, fp2);
	pFilter->bndPassByFFT_TD();
	clearGlbAry();

	delete pFilter;
}
void bndStopFilter_TD(DOUBLE_VCT wave, double fp1, double fp2)
{
	CFFT_ANSYS *pFilter = new CFFT_ANSYS(&wave, fp1, fp2);
	pFilter->bndStopByFFT_TD();
	clearGlbAry();

	delete pFilter;
}
int dataHomogenization(DOUBLE_VCT &waveData)
{
#ifdef DELETE_DC
	int i, nDataSize;
	double avgValue = 0.0;
	nDataSize = waveData.size();
	for (i = 0; i != nDataSize; i++)
	{
		avgValue += waveData[i];
	}
	avgValue /= nDataSize;
	for (i = 0; i != nDataSize; i++)
	{
		waveData[i] -= avgValue;
	}
#endif

	return 0;
}
int deleteTrending(DOUBLE_VCT dataInti, DOUBLE_VCT &dataRslt)
{
#ifdef TEST_TREND
	int i, j, k, nDataSize;
	double **vCoeffA, *vCoeffB, *result;
	vCoeffA = new double *[GLB_FIT_ORDER];
	vCoeffB = new double[GLB_FIT_ORDER];
	result = new double[GLB_FIT_ORDER];
	for (i = 0; i < GLB_FIT_ORDER; i++)
	{
		vCoeffB[i] = 0.0;
		vCoeffA[i] = new double[GLB_FIT_ORDER];
		for (j = 0; j < GLB_FIT_ORDER; j++)
			vCoeffA[i][j] = 0.0;
	}

	nDataSize = dataInti.size();
	double *vDvidedT = new double[nDataSize];
	for (i = 0; i < nDataSize; i++)
		vDvidedT[i] = i / (double)GLB_FS;
	for (i = 0; i < GLB_FIT_ORDER; i++)
	{
		for (j = 0; j < nDataSize; j++)
		{
			vCoeffB[i] += dataInti[j] * pow(vDvidedT[j], i);
		}
	}

	for (i = 0; i < GLB_FIT_ORDER; i++)
	{
		for (j = 0; j < GLB_FIT_ORDER; j++)
		{
			for (k = 0; k < nDataSize; k++)
			{
				vCoeffA[i][j] += pow(vDvidedT[k], i + j);
			}
		}
	}

	//GLB_FIT_ORDER = 4;
	//vCoeffA[0][0] = 2, vCoeffA[0][1] = 3, vCoeffA[0][2] = 4, vCoeffA[0][3] = -5;
	//vCoeffA[1][0] = 6, vCoeffA[1][1] = 7, vCoeffA[1][2] = -8, vCoeffA[1][3] = 9;
	//vCoeffA[2][0] = 10, vCoeffA[2][1] = 11, vCoeffA[2][2] = 12, vCoeffA[2][3] = 13;
	//vCoeffA[3][0] = 14, vCoeffA[3][1] = 15, vCoeffA[3][2] = 16, vCoeffA[3][3] = 17;
	//vCoeffB[0] = -6, vCoeffB[1] = 96, vCoeffB[2] = 312, vCoeffB[3] = 416;

	double *vTransfC = new double[GLB_FIT_ORDER];//存储初等行变换的系数，用于行的相减
	//消元的整个过程如下，总共GLB_FIT_ORDER-1次消元过程。
	for (i = 0; i < GLB_FIT_ORDER - 1; i++)
	{
		//求出第I次初等行变换的系数
		for (k = i + 1; k < GLB_FIT_ORDER; k++)
			vTransfC[k] = vCoeffA[k][i] / vCoeffA[i][i];

		//第I次的消元计算
		for (k = i + 1; k < GLB_FIT_ORDER; k++)
		{
			for (j = i; j < GLB_FIT_ORDER; j++)
			{
				vCoeffA[k][j] = vCoeffA[k][j] - vTransfC[k] * vCoeffA[i][j];
			}
			vCoeffB[k] = vCoeffB[k] - vTransfC[k] * vCoeffB[i];
		}
	}
	//解的存储数组
	//先计算出最后一个未知数；
	result[GLB_FIT_ORDER - 1] = vCoeffB[GLB_FIT_ORDER - 1] / vCoeffA[GLB_FIT_ORDER - 1][GLB_FIT_ORDER - 1];
	//求出每个未知数的值
	for (i = GLB_FIT_ORDER - 2; i >= 0; i--)
	{
		double sum = 0;
		for (j = i + 1; j < GLB_FIT_ORDER; j++)
		{
			sum += vCoeffA[i][j] * result[j];
		}
		result[i] = (vCoeffB[i] - sum) / vCoeffA[i][i];
	}
	//////////////////////////////////////////////////////
	for (i = 0; i < nDataSize; i++)
	{
		double dTrend = result[0];
		for (j = 1; j < GLB_FIT_ORDER; j++)
		{
			dTrend += result[j] * pow(vDvidedT[i], j);
		}
		dataRslt.push_back(dataInti[i] - dTrend);
	}
	//////////////////////////////////////////////////////
	for (i = 0; i < GLB_FIT_ORDER; i++)
	{
		delete[]vCoeffA[i];
		vCoeffA[i] = NULL;
	}
	delete[]vCoeffA; vCoeffA = NULL;
	delete[]vCoeffB; vCoeffB = NULL;
	delete[]vTransfC; vTransfC = NULL;
	delete[]vDvidedT; vDvidedT = NULL;
	delete[]result; result = NULL;

#endif

	return 0;
}

