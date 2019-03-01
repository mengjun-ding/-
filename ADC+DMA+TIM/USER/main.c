#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"

u16 ADC_Buff0[1024];//4^5
u16 ADC_Buff1[1024];//4^5
u16 samples[1024];
int main(void)
{ 
	u16 i;
	delay_init(168);		  //��ʼ����ʱ����
	uart_init(115200);	//���ڳ�ʼ��������Ϊ115200
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	LED_Init();		        //��ʼ��LED�˿�
	Adc_Init();
	
	while(1)
	{
		delay_ms(1000);
		for(i=0;i<1024;i++)
		{
			samples[i]=ADC_Buff0[i];
		}
		for(i=0;i<1024;i++)
		{
			printf("%d\r\n",samples[i]);
		}
//		for(i=0;i<24;i++)
//		{
//			printf("%d\r\n",ADC_Buff0[i]);
//			printf("%d\r\n",ADC_Buff1[i]);
//		}
		delay_ms(1000);
	}
}

