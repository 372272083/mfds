#pragma once

#include "math.h"
#include "windows.h"
#include "vector"


#define    ONE_PI           3.1415926535898
#define    TWO_PI			6.2831853071796
#define    DEFnum           16384

const double MAX_DOU_VALUE = 1E+10;
const double MIN_DOU_VALUE = -1E+10;

const double SQRT_2 = 1.414213562373095;
const double SQRT_3 = 1.732050807568877;
const double HALF_SQRT_2 = 0.7071067811865475;
const double HALF_SQRT_3 = 0.8660254037844386;


enum EnWindowType{ WindowTypeNone = 0,
				   RectangleWindow,
				   HanningWindow,
				   HammingWindow,
				   BlackmanWindow,
};

enum EnMotorType{ MotorTypeNone = 0, synchronousMotor, asynchronousMotors };
enum EnWiringType{ WiringTypeNone = 0, Y_Type, A_Type };
enum EnInsulationLevel{ InsulationLevelNone = 0 };
enum EnSystemMode{ ModeNone = 0, studyMode, diagnosisMode };
enum EnUserCtrlMode{ UserCtrlNone = 0, studyUserCtrl, diagnosisUserCtrl };

enum EnBearType{ BearTypeNone = 0 };
enum EnBearModel{ BearModeNone = 0 };
enum EnInputTypeLib{ TypeNone = 0 };

enum EnLoadType{ loadNone = 0, SteadyLoad, unSteadyLoad };
enum EnRunCondition{ ConditionNone = 0 };
enum EnFreqAnsysType{ AnsysTypeNone = 0, TypeFFT, TypeDFT };

struct SComplex
{
	double r, i;
	SComplex() 	{ r = 0.0; i = 0.0; }

	SComplex operator=(SComplex op)
	{
		r = op.r;
		i = op.i;
		return *this;
	}
	SComplex operator+(SComplex op)
	{
		SComplex tmp;
		tmp.r = r + op.r;
		tmp.i = i + op.i;
		return tmp;
	}
	SComplex operator+=(SComplex op)
	{
		r += op.r; i += op.i;
		return *this;
	}
	SComplex operator-(SComplex op)
	{
		SComplex tmp;
		tmp.r = r - op.r;
		tmp.i = i - op.i;
		return tmp;
	}
	SComplex operator*(SComplex op)
	{
		SComplex tmp;
		tmp.r = r * op.r - i * op.i;
		tmp.i = r * op.i + i * op.r;
		return tmp;
	}
	SComplex operator/(SComplex op)
	{
		SComplex tmp;
		double t = op.r*op.r + op.i*op.i;
		tmp.r = (r * op.r + i * op.i) / t;
		tmp.i = (i * op.r - r * op.i) / t;
		return tmp;
	}
};


typedef std::vector<double> DOUBLE_VCT;
typedef std::vector<SComplex> COMPLEX_VCT;


