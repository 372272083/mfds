#include "stdafx.h"
#include "globals.h"
#include "FFT_ANSYS.h"
//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 

////////////////////////////////////////////////
CFFT_ANSYS::CFFT_ANSYS(DOUBLE_VCT vWaveData0,
					   EnWindowType windowType0)
{
	askCurrentTime(FFT_str);
	initialize();

	vWaveVct = vWaveData0;
	windowType = windowType0;

	FFT_Num = vWaveVct.size();
	double *windowCoeff = new double[FFT_Num];
	getWindowCoefficient(windowType, windowCoeff, FFT_Num);
	for (int i = 0; i < FFT_Num; i++)
	{
		Glb_Cmp_Ary[i].r = vWaveVct[i] * windowCoeff[i];
	}
	delete[]windowCoeff;
	FFT_Layer = log(FFT_Num*1.0) / log(2.0);
}
CFFT_ANSYS::CFFT_ANSYS(double vWaveAry0[],
					   int FFT_Num0,
					   double fp1,
					   double fp2)
{
	askCurrentTime(FFT_str);
	initialize();

	vWaveAry = vWaveAry0;
	FFT_Num = FFT_Num0;
	freq1 = fp1;
	freq2 = fp2;
	FFT_Layer = log(FFT_Num*1.0) / log(2.0);
}

CFFT_ANSYS::CFFT_ANSYS(DOUBLE_VCT *pWaveVct0,
					   double fp1,
					   double fp2)
{
	askCurrentTime(FFT_str);
	initialize();

	pWaveVct = pWaveVct0;
	freq1 = fp1;
	freq2 = fp2;
	FFT_Num = pWaveVct->size();
	FFT_Layer = log(FFT_Num*1.0) / log(2.0);
}
CFFT_ANSYS::~CFFT_ANSYS()
{
}
void CFFT_ANSYS::initialize()
{
	FFT_Layer = -1;
	FFT_Num = -1;
	vWaveAry = NULL;
}
int CFFT_ANSYS::FFTAnalysis()
{
	int i, j, k;
	if (FFT_Num == 0) return 0;
	for (i = 0, j = 0; i < FFT_Num - 1; i++)
	{
		if (i < j)
		{
			SComplex temp_Complex;
			temp_Complex = Glb_Cmp_Ary[i];
			Glb_Cmp_Ary[i] = Glb_Cmp_Ary[j];
			Glb_Cmp_Ary[j] = temp_Complex;
		}
		k = FFT_Num >> 1;
		while (k <= j)
		{
			j = j - k;
			k >>= 1;
		}
		j = j + k;
	}
////////////////////////////////////////////////
	SComplex *vW_nk = new SComplex[FFT_Num / 2];
	double tmp1, tmp2;
	tmp1 = TWO_PI / FFT_Num;
	for (i = 0; i < FFT_Num / 2; i++)
	{
		tmp2 = tmp1 * i;
		vW_nk[i].r = cos(tmp2);
		vW_nk[i].i = -sin(tmp2);
	}
////////////////////////////////////////////////
	int cur_layer, up_idx, dw_idx, dis_grp, num_grp, cur_grp, W_nk_idx;
	for (cur_layer = 1; cur_layer <= FFT_Layer; cur_layer++)
	{
		num_grp = (int)pow(2.0, cur_layer);
		dis_grp = num_grp / 2;
		for (cur_grp = 1; cur_grp <= dis_grp; cur_grp++)
		{
			W_nk_idx = (cur_grp - 1) * (int)pow(2.0, FFT_Layer - cur_layer);
			for (up_idx = cur_grp - 1; up_idx < FFT_Num - 1; up_idx = up_idx + num_grp)
			{
				dw_idx = up_idx + dis_grp;  //up_idx,dw_idx分别代表一个碟形单元的上、下节点编号   

				SComplex tmp_up = Glb_Cmp_Ary[up_idx];
				SComplex tmp_dw = Glb_Cmp_Ary[dw_idx];

				Glb_Cmp_Ary[up_idx] = tmp_up + vW_nk[W_nk_idx] * tmp_dw;
				Glb_Cmp_Ary[dw_idx] = tmp_up - vW_nk[W_nk_idx] * tmp_dw;
			}
		}
	}
	delete[]vW_nk;
//////////////////////////////////////////////
	//SComplex *vMATLAB_READ = new SComplex[FFT_Num];
	//FILE *stream = fopen("C:\\test.txt", "r");
	//for (i = 0; !feof(stream); ++i)
	//{
	//	fscanf(stream, "%lf", &vMATLAB_READ[i].r);
	//	fscanf(stream, "%lf", &vMATLAB_READ[i].i);
	//}
	//_ASSERT(i == FFT_Num);
	//for (i = 0; i != FFT_Num; i++)
	//{
	//	_ASSERT(abs(vMATLAB_READ[i].r - Glb_Cmp_Ary[i].r) < 1e-6);
	//	_ASSERT(abs(vMATLAB_READ[i].i - Glb_Cmp_Ary[i].i) < 1e-6);
	//}
	//delete[]vMATLAB_READ;
////////////////////////////////////////////////
	askCurrentTime(FFT_end);
	dTime = costTime(FFT_str, FFT_end);

	return 0;
}
//int CFFT_ANSYS::getAmptAndPhase()
//{
//	_ASSERT(FFT_Num == 16384);
//	for (int i = 0; i != 8192; i++)
//	{
//		double tmpAmpt = getAmplitude(Glb_Cmp_Ary[i]);
//		AmptFreq[i] = tmpAmpt / 8192;
//
//		phasFreq[i] = getPhaseAngle(Glb_Cmp_Ary[i]);
//	}
//	AmptFreq[0] /= 2.0;
//
//	return 0;
//}
/*************************************************************/
int CFFT_ANSYS::IFFTAnalysis()
{
	if (FFT_Num == 0) return 0;

	int i, j, k;
	SComplex *vW_nk = new SComplex[FFT_Num / 2];
	double tmp1, tmp2;
	tmp1 = TWO_PI / FFT_Num;
	for (i = 0; i < FFT_Num / 2; i++)
	{
		tmp2 = tmp1 * i;
		vW_nk[i].r = cos(tmp2);
		vW_nk[i].i = -sin(tmp2);
	}

	int m = FFT_Num;
	for (i = 0; i != FFT_Layer; i++)
	{
		m /= 2;
		for (j = 0; j < FFT_Num; j += 2 * m)
		{
			for (k = 0; k < m; k++)
			{
				int idx_up = j + k;
				int idx_dw = j + k + m;

				SComplex tmp_up = Glb_Cmp_Ary[idx_up] + Glb_Cmp_Ary[idx_dw];
				SComplex tmp_dw = Glb_Cmp_Ary[idx_up] - Glb_Cmp_Ary[idx_dw];
				
				tmp_up.i /= 2; tmp_up.r /= 2;
				tmp_dw.i /= 2; tmp_dw.r /= 2;

				tmp_dw = tmp_dw / vW_nk[FFT_Num*k / 2 / m];

				Glb_Cmp_Ary[idx_up] = tmp_up;
				Glb_Cmp_Ary[idx_dw] = tmp_dw;
			}
		}
	}
	for (i = 0, j = 0; i < FFT_Num - 1; i++)
	{
		if (i < j)
		{
			SComplex temp_Complex;
			temp_Complex = Glb_Cmp_Ary[i];
			Glb_Cmp_Ary[i] = Glb_Cmp_Ary[j];
			Glb_Cmp_Ary[j] = temp_Complex;
		}
		k = FFT_Num >> 1;
		while (k <= j)
		{
			j = j - k;
			k >>= 1;
		}
		j = j + k;
	}
	delete[]vW_nk;

	return 0;
}
///////////////////////////////////////////
int CFFT_ANSYS::lowPassByFFT_TD()
{
	int i;
	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveAry[i];
		}
	}
	else
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveVct[i];
		}
	}
	FFTAnalysis();

	int iStr = ceil(2 * freq1);
	for (i = 0; i != FFT_Num; i++)
	{
		if (i<iStr || i>FFT_Num - iStr) 
			continue;
		else
		{
			Glb_Cmp_Ary[i].r = 0.0;
			Glb_Cmp_Ary[i].i = 0.0;
		}
	}	
	IFFTAnalysis();

	if (vWaveAry != NULL)
	{
		for (i = 0; i != FFT_Num; i++)
		{
			vWaveAry[i] = Glb_Cmp_Ary[i].r;
		}
	}
	else
	{
		for (i = 0; i != FFT_Num; i++)
		{
			vWaveVct[i] = Glb_Cmp_Ary[i].r;
		}
	}

	return 0;
}
int CFFT_ANSYS::hghPassByFFT_TD()
{
	int i;
	double deltaF = GLB_FS / FFT_Num;

	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveAry[i];
		}
	}
	else
	{
        //_ASSERT(pWaveVct->size() != 0);
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = (*pWaveVct)[i];
		}
	}

	FFTAnalysis();
	int iEnd = ceil(deltaF * freq1);
	for (i = 0; i != iEnd+1; i++)
	{
		Glb_Cmp_Ary[i].r = 0.0;
		Glb_Cmp_Ary[i].i = 0.0;
	}
	int iStr = FFT_Num - iEnd;
	for (i = iStr; i != FFT_Num; i++)
	{
		Glb_Cmp_Ary[i].r = 0.0;
		Glb_Cmp_Ary[i].i = 0.0;
	}
	IFFTAnalysis();

	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			vWaveAry[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-8);
		}
	}
	else
	{
        //_ASSERT(pWaveVct->size() != 0);
		for (i = 0; i < FFT_Num; i++)
		{
			(*pWaveVct)[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-8);
		}
	}
	
	return 0;
}
int CFFT_ANSYS::bndPassByFFT_TD()
{
	int i;
	double deltaF = GLB_FS / FFT_Num;

	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveAry[i];
		}
	}
	else
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = (*pWaveVct)[i];
		}
	}
	FFTAnalysis();
	int iStr = ceil(deltaF * freq1);
	int iEnd = floor(deltaF * freq2);
	for (i = 0; i != FFT_Num; i++)
	{
		if ((iStr <= i&& i <= iEnd) || (FFT_Num-iEnd <= i&& i <= FFT_Num-iStr))
			continue;
		else
		{
			Glb_Cmp_Ary[i].r = 0.0;
			Glb_Cmp_Ary[i].i = 0.0;
		}
	}
	IFFTAnalysis();
	
	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			vWaveAry[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-8);
		}
	}
	else
	{
		for (i = 0; i < FFT_Num; i++)
		{
			(*pWaveVct)[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-8);
		}
	}

	return 0;
}
int CFFT_ANSYS::bndStopByFFT_TD()
{
	int i;
	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveAry[i];
		}
	}
	else
	{
		for (i = 0; i < FFT_Num; i++)
		{
			Glb_Cmp_Ary[i].r = vWaveVct[i];
		}
	}
	FFTAnalysis();

	int iStr = ceil(2 * freq1);
	int iEnd = floor(2 * freq2);

	for (i = iStr; i != iEnd; i++)
	{
		Glb_Cmp_Ary[i].r = 0.0;
		Glb_Cmp_Ary[i].i = 0.0;
	}
	for (i = FFT_Num - iEnd; i != FFT_Num - iStr; i++)
	{
		Glb_Cmp_Ary[i].r = 0.0;
		Glb_Cmp_Ary[i].i = 0.0;
	}
	IFFTAnalysis();

	if (vWaveAry != NULL)
	{
		for (i = 0; i < FFT_Num; i++)
		{
			vWaveAry[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-6);
		}
	}
	else
	{
		for (i = 0; i < FFT_Num; i++)
		{
			vWaveVct[i] = Glb_Cmp_Ary[i].r;
            //_ASSERT(Glb_Cmp_Ary[i].i < 1e-6);
		}
	}

	return 0;
}






