#include "sys.h"
#include "common.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"
#include "dac.h"
#include "math.h" 
#include "arm_math.h"  
#include "fft.h"
#include "string.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
#include "gui.h"

u16 fliter_switch=0;

u16 ADC_Buff0[SAMPLE_LENGTH];//4^5
u16 ADC_Buff1[SAMPLE_LENGTH];//4^5
u16 dacwave_buff0[SAMPLE_LENGTH]={0};
u16 dacwave_buff1[SAMPLE_LENGTH]={0};
float samples[SAMPLE_LENGTH];//为了不占用缓冲区 直接进行复制

float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT输入数组
float fft_outputmag[SAMPLE_LENGTH];	//FFT输出数组
float fft_inputbuff[SAMPLE_LENGTH];	//FFT输出数组
float dsp_signal[SAMPLE_LENGTH];

u16 wave[WAVE_LENGTH]={109,119,129,138,147,156,164,171,178,184,189,193,196,198,199,199,199,197,194,190,186,180,174,
	167,159,151,142,133,123,114,104,94,84,74,64,55,47,38,31,24,18,12,8,4,2,0,0,0,1,4,7,11,16,22,29,36,44,53,62,72,81,91,
	101,111,121,131,140,149,157,165,172,179,185,189,193,196,198,199,199,198,196,194,190,185,179,173,166,158,150,141,131,
	122,112,102,92,82,72,63,54,45,37,30,23,17,12,7,4,1,0,0,0,2,4,8,12,17,23,30,38,46,55,64,73,83,93,103,113,123,132,142,
	150,159,166,174,180,185,190,194,197,199,199,199,198,196,193,189,184,178,172,165,157,148,139,130,120,110,100,90,80,
	71,61,52,44,36,28,22,16,11,7,3,1,0,0,0,2,5,8,13,18,24,31,39,47,56,65,75,85,95,105,114,124,134,143,152,160,168,175,
	181,186,191,194,197,199,199,199,198,196,192,188,183,177,171,163,155,147,138,128,118,109,99,89,79,69,60,51,42,34,
	27,21,15,10,6,3,1,0,0,0,2,5,9,14,19,26,33,40,49,58,67,76,86,96,106,116,126,135,145};
int main(void)
{ 
	u16 i;
	delay_init(168);		  //初始化延时函数
	uart_init(115200);	//串口初始化波特率为115200
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	LED_Init();		        //初始化LED端口
	Dac1_Init();
	Adc_Init();
	
	LCD_Init();
	LCDGUI_Init();
	
	KEY_Init();
	EXTIX_Init();
	while(1)
	{
		delay_ms(1000);
		
		memcpy(samples,fft_outputmag,sizeof(samples));
		for(i=0;i<WAVE_LENGTH;i++)
		{
			wave[i]=(samples[2*i]+samples[2*i+1])/800;
		}
//		for(i=0;i<WAVE_LENGTH;i++)
//		{
//			printf("%3d:%4d\r\n",i,wave[i]);
//		}
		LCD_plotwave(wave);
		
		memcpy(samples,dacwave_buff1,sizeof(samples));
//		for(i=0;i<SAMPLE_LENGTH;i++)
//		{
//			printf("%d\r\n",dacwave_buff1[i]);
//		}
//		memcpy(samples,fft_outputmag,sizeof(samples));
//		for(i=0;i<WAVE_LENGTH;i++)
//		{
//			wave[i]=fft_outputmag[2*i]+fft_outputmag[2*i+1]
//		}
//		LCD_plotwave(wave);
//		printf("fft 运算后的绝对值");
//		for(i=0;i<SAMPLE_LENGTH;i++)
//		{
//			printf("%f\r\n",samples[i]);
//		}
		
	}
}

