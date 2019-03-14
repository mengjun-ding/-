#ifndef _FFT__H
#define _FFT__H

#define FILTER_ON

void fft_1024(float* outputmag,float* output,float* real_input);
void ifft_fliter_1024(float* outputmag,float* output,float* complex_input);

#endif

