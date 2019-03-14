#include "fft.h"
#include "sys.h"
#include "common.h"
#include "math.h" 
#include "arm_math.h"  
#include "usart.h"

//extern float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT输入数组
//extern float fft_inputbuff[SAMPLE_LENGTH];	//FFT输出数组

extern u16 fliter_switch;

void findmax(float* arr,float *value,u32* pos)
{
	float max = 0;
	u16 i;
	for(i=1;i<SAMPLE_LENGTH/2;i++)//直流不需要纳入 结果是对称一般即可
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
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,0,1);//初始化scfft结构体，设定FFT相关参数
	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		output[2*i]=real_input[i];//这里的output到目前为止是input
		output[2*i+1]=0;
	}
	arm_cfft_radix4_f32(&scfft,output);	//FFT计算（基4）做过计算后input变成了output
	arm_cmplx_mag_f32(output,outputmag,SAMPLE_LENGTH);	//把运算结果复数求模得幅值

}
void ifft_fliter_1024(float* outputmag,float* output,float* complex_input)
{
	arm_cfft_radix4_instance_f32 scfft;
	u16 i;u16 j;
	u32 max;float value;
//	#ifdef FILTER_ON//滤波
	//大于50000 斩断 直流除外
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
	
		//寻找高峰
		findmax(outputmag,&value,&max);
		
	//	printf("\r\nvalue=%f\r\n\r\n",value);
		for(i=0;i<11;i++)
		{
			complex_input[2*(84+i-5)]=0;
			complex_input[2*(84+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(84+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(84+i-5))+1]=0;//像是前级自带的3.2k噪声
			
			complex_input[2*(160+i-5)]=0;
			complex_input[2*(160+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(160+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(160+i-5))+1]=0;//2次
			
			complex_input[2*(192+i-5)]=0;
			complex_input[2*(192+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(192+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(192+i-5))+1]=0;
			
			complex_input[2*(256+i-5)]=0;
			complex_input[2*(256+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(256+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(256+i-5))+1]=0;//3次
			
			complex_input[2*(max+i-5)]=0;
			complex_input[2*(max+i-5)+1]=0;
			complex_input[2*(SAMPLE_LENGTH-(max+i-5))]=0;
			complex_input[2*(SAMPLE_LENGTH-(max+i-5))+1]=0;
		}
	}
//	#endif
	
	arm_cfft_radix4_init_f32(&scfft,SAMPLE_LENGTH,1,1);//初始化scfft结构体，设定FFT相关参数 第三个参数意思是ifft
	arm_cfft_radix4_f32(&scfft,complex_input);	//FFT计算（基4）做过计算后input变成了output
	arm_cmplx_mag_f32(complex_input,output,SAMPLE_LENGTH);	//把运算结果复数求模得幅值
}

