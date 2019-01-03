#include "Basic.hpp"

#define DELETE_DC////
#define TEST_TREND//
#define INPUT_DATAS_TEST//

//全局变量
///////////////////////////////////////////////////////
const int GLB_FS = 16384;
const int GLB_FIT_ORDER = 4;
extern SComplex Glb_Cmp_Ary[GLB_FS];
void clearGlbAry();
///////////////////////////////////////////////////////

void askCurrentTime(LARGE_INTEGER& curTime);
double costTime(LARGE_INTEGER str, LARGE_INTEGER end);

double getPhaseAngle(SComplex cmp);
double getAmplitude(SComplex cmp);
double getAmplitude(double image, double real);

void getHanningCoefficients(double *vCoeff, int num);
void getHammingCoefficients(double *vCoeff, int num);
int getWindowCoefficient(EnWindowType windowType,  double *coeff,  int num);

int lowPassFilter_TD(double wave[], int num, double fp);
int hghPassFilter_TD(double wave[], int num, double fp);
int bndPassFilter_TD(double wave[], int num, double fp1, double fp2);
int bndStopFilter_TD(double wave[], int num, double fp1, double fp2);

int lowPassFilter_FD(SComplex freq[], int num, double fp);
int hghPassFilter_FD(SComplex freq[], int num, double fp);
int bndPassFilter_FD(SComplex freq[], int num, double fp1, double fp2);
int bndStopFilter_FD(SComplex freq[], int num, double fp1, double fp2);

///**********************************************************///
void lowPassFilter_TD(DOUBLE_VCT &wave, double fp);
void hghPassFilter_TD(DOUBLE_VCT &wave, double fp);
void bndPassFilter_TD(DOUBLE_VCT &wave, double fp1, double fp2);
void bndStopFilter_TD(DOUBLE_VCT &wave, double fp1, double fp2);
///**********************************************************///

int dataHomogenization(DOUBLE_VCT &waveData);//零均质化处理
int deleteTrending(DOUBLE_VCT dataInti, DOUBLE_VCT &dataRslt);