#include "fft.h"
#include "sys.h"
#include "common.h"
#include "math.h" 
#include "arm_math.h"  

extern float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT��������
extern float fft_inputbuff[SAMPLE_LENGTH];	//FFT�������

void fft_1024(float* output,float* real_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;
	float input[SAMPLE_LENGTH*2];
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,0,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		input[2*i]=real_input[i];
		input[2*i+1]=0;
	}
	arm_cfft_radix4_f32(&scfft,input);	//FFT���㣨��4�����������input�����output
	
	for(i=0;i<2*SAMPLE_LENGTH;i++)
	{
		output[i]=input[i];
	}//output �����������
}
void ifft_fliter_1024(float* output,float* complex_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,1,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز��� ������������˼��ifft
	arm_cfft_radix4_f32(&scfft,complex_input);	//FFT���㣨��4�����������input�����output
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		output[i]=complex_input[2*i];
	}//output ������dsp����ź�
}

