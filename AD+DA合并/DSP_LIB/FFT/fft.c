#include "fft.h"
#include "sys.h"
#include "common.h"
#include "math.h" 
#include "arm_math.h"  

extern float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT输入数组
extern float fft_inputbuff[SAMPLE_LENGTH];	//FFT输出数组

void fft_1024(float* output,float* real_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;
	float input[SAMPLE_LENGTH*2];
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		input[2*i]=real_input[i];
		input[2*i+1]=0;
	}
	arm_cfft_radix4_f32(&scfft,input);	//FFT计算（基4）做过计算后input变成了output
	
	for(i=0;i<2*SAMPLE_LENGTH;i++)
	{
		output[i]=input[i];
	}//output 现在是输出了
}
void ifft_fliter_1024(float* output,float* complex_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数 第三个参数意思是ifft
	arm_cfft_radix4_f32(&scfft,complex_input);	//FFT计算（基4）做过计算后input变成了output
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		output[i]=complex_input[2*i];
	}//output 现在是dsp后的信号
}

