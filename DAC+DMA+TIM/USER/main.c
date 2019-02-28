#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dac.h"

const u16 sinwave[100]={2047,2177,2306,2434,2561,2686,2808,2927,3042,3154,3261,3363,3460,3551,3636,3715,3787,
	3852,3909,3960,4002,4037,4063,4082,4092,4094,4088,4074,4051,4020,3982,3936,3882,3820,3752,3676,3594,3506,
3412,3313,3208,3099,2985,2868,2747,2624,2498,2371,2242,2112,1982,1852,1723,1596,1470,1347,1226,1109,995,886,
781,682,588,500,418,342,274,212,158,112,74,43,20,6,0,2,12,31,57,92,134,185,242,307,379,458,543,
634,731,833,940,1052,1167,1286,1408,1533,1660,1788,1917,2047};//一百点正弦
	
int main(void)
{ 
 
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	uart_init(115200);    //串口初始化 波特率115200
	Dac1_Init();
	while(1)
	{
		printf("程序正在运行\r\n");
		delay_ms(1000);
	}
}
