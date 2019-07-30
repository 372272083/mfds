#ifndef FFTPARAMS_H
#define FFTPARAMS_H

#include "constants.h"

class FFTParams
{
public:
    FFTParams();

    float filter_lower;
    float filter_up;
    FFT_WINDOW fft_w;
    FFT_FilterType fft_filter;
};

#endif // FFTPARAMS_H
