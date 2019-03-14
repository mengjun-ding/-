#include "fft.h"
#include "sys.h"
#include "common.h"
#include "math.h" 
#include "arm_math.h"  
#include "usart.h"

//extern float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT��������
//extern float fft_inputbuff[SAMPLE_LENGTH];	//FFT�������

extern u16 fliter_switch;

void findmax(float* arr,float *value,u32* pos)
{
	float max = 0;
	u16 i;
	for(i=1;i<SAMPLE_LENGTH/2;i++)//ֱ������Ҫ���� ����ǶԳ�һ�㼴��
	{
		if(arr[i]>max)
		{
		  max=arr[i];
			*pos =i;
		}
	}
	*value=max;
}

void fft_1024(float* outputmag,float* output,float* real_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;
//	float input[SAMPLE_LENGTH*2];
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,0,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		output[2*i]=real_input[i];//�����output��ĿǰΪֹ��input
		output[2*i+1]=0;
	}
	arm_cfft_radix4_f32(&scfft,output);	//FFT���㣨��4�����������input�����output
	arm_cmplx_mag_f32(output,outputmag,SAMPLE_LENGTH);	//��������������ģ�÷�ֵ

}
void ifft_fliter_1024(float* outputmag,float* output,float* complex_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;u16 j;
	u32 max;float value;
//	#ifdef FILTER_ON//�˲�
	//����50000 ն�� ֱ������
	if(fliter_switch)
	{
		for(i=1;i<SAMPLE_LENGTH;i++)
		{
			if(outputmag[i]>50000)
			{
				if(outputmag[i]>80000)
				{
					for(j=0;j<5;j++)
					{
						complex_input[2*(i+j-2)]/=500;
						complex_input[2*(i+j-2)+1]/=500;
					}
				}
				else
				{
					for(j=0;j<5;j++)
					{
						complex_input[2*(i+j-2)]/=50;
						complex_input[2*(i+j-2)+1]/=50;
					}
				}

			}
		}
	
		//Ѱ�Ҹ߷�
		findmax(outputmag,&value,&max);
		
	//	printf("\r\nvalue=%f\r\n\r\n",value);
		for(i=0;i<11;i++)
		{
			complex_input[2*(84+i-5)]=0;
			complex_input[2*(84+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(84+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(84+i-5))+1]=0;//����ǰ���Դ���3.2k����
			
			complex_input[2*(160+i-5)]=0;
			complex_input[2*(160+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(160+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(160+i-5))+1]=0;//2��
			
			complex_input[2*(192+i-5)]=0;
			complex_input[2*(192+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(192+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(192+i-5))+1]=0;
			
			complex_input[2*(256+i-5)]=0;
			complex_input[2*(256+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(256+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(256+i-5))+1]=0;//3��
			
			complex_input[2*(max+i-5)]=0;
			complex_input[2*(max+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(max+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(max+i-5))+1]=0;
		}
	}
//	#endif
	
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,1,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز��� ������������˼��ifft
	arm_cfft_radix4_f32(&scfft,complex_input);	//FFT���㣨��4�����������input�����output
	arm_cmplx_mag_f32(complex_input,output,SAMPLE_LENGTH);	//��������������ģ�÷�ֵ
}

