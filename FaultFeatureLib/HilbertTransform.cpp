#include "stdafx.h"
#include "HilbertTransform.h"
#include "FFT_ANSYS.h"
#include "globals.h"

//////////////////////////////////////////////////
#ifdef _DEBUG  
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
#endif 
//////////////////////////////////////////////////
CHilbert::CHilbert(DOUBLE_VCT *pWaveData0)
{
	pWaveData = pWaveData0;
	FFT_Num = pWaveData->size();
}	
int CHilbert::reGetWaveEnvelope()
{
	return 0;
}
CHilbert::~CHilbert()
{
}
int CHilbert::hilbertTransform_mthd_1()
{

//[[FFT
	int i, j, k, FFT_Layer;
	for (int i = 0; i < FFT_Num; i++)
	{
		Glb_Cmp_Ary[i].r = (*pWaveData)[i];
	}
	FFT_Layer = log(FFT_Num) / log(2);
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

	double tmp1, tmp2;
	tmp1 = TWO_PI / FFT_Num;
	SComplex *vW_nk = new SComplex[FFT_Num / 2];
	for (i = 0; i < FFT_Num / 2; i++)
	{
		tmp2 = tmp1 * i;
		vW_nk[i].r = cos(tmp2);
		vW_nk[i].i = -sin(tmp2);
	}

	int cur_layer, up_idx, dw_idx, dis_grp, num_grp, cur_grp, W_nk_idx;
	for (cur_layer = 1; cur_layer <= FFT_Layer; cur_layer++)
	{
		num_grp = pow(2, cur_layer);
		dis_grp = num_grp / 2;
		for (cur_grp = 1; cur_grp <= dis_grp; cur_grp++)
		{
			W_nk_idx = (cur_grp - 1) * pow(2, FFT_Layer - cur_layer);
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
//]]
//////////////////////////////////////////////////////////////////////
//[[频域处理
	for (i = 0; i != FFT_Num; i++)
	{
		double tmp = Glb_Cmp_Ary[i].r;
		if (i >= FFT_Num / 2)
		{
			Glb_Cmp_Ary[i].r = -Glb_Cmp_Ary[i].r;
			Glb_Cmp_Ary[i].i = -Glb_Cmp_Ary[i].i;
		}
	}
//]]
//////////////////////////////////////////////////////////////////////
//[[IFFT
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
//]]
	for (i = 0; i < FFT_Num; i++)
	{
		(*pWaveData)[i] = Glb_Cmp_Ary[i].i;
	}
	return 0;
}
int CHilbert::hilbertTransform_mthd_2()
{
	int i, n1, n2;
	double t;
	n1 = FFT_Num / 2;
	n2 = n1 + 1;
	hartleyTransform();
	for (i = 1; i<n1; i++)
	{
		t = (*pWaveData)[i];
		(*pWaveData)[i] = (*pWaveData)[FFT_Num - i];
		(*pWaveData)[FFT_Num - i] = t;
	}
	for (i = n2; i<FFT_Num; i++)
		(*pWaveData)[i] = -(*pWaveData)[i];
	(*pWaveData)[0] = 0.0;
	(*pWaveData)[n1] = 0.0;
	hartleyTransform();
	t = 1.0 / FFT_Num;
	for (i = 0; i<FFT_Num; i++)
		(*pWaveData)[i] *= t;
	return 0;
}
int CHilbert::hartleyTransform()//哈特莱变换
{
	int i, j, k, m, l1, l2, l3, l4, n1, n2, n4;
	double a, e, c, s, t, t1, t2;
	for (j = 1, i = 1; i<16; i++)
	{
		m = i;
		j = 2 * j;
		if (j == FFT_Num) break;
	}
	n1 = FFT_Num - 1;
	for (j = 0, i = 0; i<n1; i++)
	{
		if (i<j)
		{
			t = (*pWaveData)[j];
			(*pWaveData)[j] = (*pWaveData)[i];
			(*pWaveData)[i] = t;
		}
		k = FFT_Num / 2;
		while (k<(j + 1))
		{
			j = j - k;
			k = k / 2;
		}
		j = j + k;
	}
	for (i = 0; i<FFT_Num; i += 2)
	{
		t = (*pWaveData)[i];
		(*pWaveData)[i] = t + (*pWaveData)[i + 1];
		(*pWaveData)[i + 1] = t - (*pWaveData)[i + 1];
	}
	n2 = 1;
	for (k = 2; k <= m; k++)
	{
		n4 = n2;
		n2 = n4 + n4;
		n1 = n2 + n2;
		e = 6.28318530719586 / n1;
		for (j = 0; j<FFT_Num; j += n1)
		{
			l2 = j + n2;
			l3 = j + n4;
			l4 = l2 + n4;
			t = (*pWaveData)[j];
			(*pWaveData)[j] = t + (*pWaveData)[l2];
			(*pWaveData)[l2] = t - (*pWaveData)[l2];
			t = (*pWaveData)[l3];
			(*pWaveData)[l3] = t + (*pWaveData)[l4];
			(*pWaveData)[l4] = t - (*pWaveData)[l4];
			a = e;
			for (i = 1; i<n4; i++)
			{
				l1 = j + i;
				l2 = j - i + n2;
				l3 = l1 + n2;
				l4 = l2 + n2;
				c = cos(a);
				s = sin(a);
				t1 = (*pWaveData)[l3] * c + (*pWaveData)[l4] * s;
				t2 = (*pWaveData)[l3] * s - (*pWaveData)[l4] * c;
				a = (i + 1)*e;
				t = (*pWaveData)[l1];
				(*pWaveData)[l1] = t + t1;
				(*pWaveData)[l3] = t - t1;
				t = (*pWaveData)[l2];
				(*pWaveData)[l2] = t + t2;
				(*pWaveData)[l4] = t - t2;
			}
		}
	}
	return 0;
}


