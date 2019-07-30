#include "fftparams.h"

FFTParams::FFTParams()
{
    filter_lower = 10.0;
    filter_up = -1;
    fft_w = WINDOW_RECT;
    fft_filter = HghPassFilter;
}
