#ifndef __DAC_H
#define __DAC_H	 
#include "sys.h"	     			    

#define DAC_DHR12R1_ADDRESS 

void Dac1_Init(void);		//DACͨ��1��ʼ��	 	 
void Dac1_Set_Vol(u16 vol);	//����ͨ��1�����ѹ
#endif
