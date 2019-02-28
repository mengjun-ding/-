#include "sys.h"
#include "adc.h"
#include "usart.h"

extern u16 ADC_Buff0[1024];//4^5
extern u16 ADC_Buff1[1024];//4^5


//最好改成定时器触发的单次转换 以匹配DAC的转换 实现转换的同步
void Adc_Init(void)
{
	 
	GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;    
  NVIC_InitTypeDef NVIC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;    
	
	
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
	
	//DMA2 channel0配置
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;    //ADC1是channel0
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Buff0[0];//定义的数组
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);//adc的DR寄存器基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//从外设到存储器
	DMA_InitStructure.DMA_BufferSize = 1024;                   //传输1024个值
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          //存储器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设半字传输 一次2个字节
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //存储器也是2个字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//不使用FIFO      
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_DoubleBufferModeConfig(DMA2_Stream0,(uint32_t)&ADC_Buff1,DMA_Memory_1);//这个ADC_Buff0是memory0  ADC_Buff1是memory1 
	DMA_DoubleBufferModeCmd(DMA2_Stream0,ENABLE);//开辟双缓冲
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);//开启传输
	
 //设置DMA中断
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC); //清除中断标志 
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE); //传输完成中断 允许更新中断（必要）                              
	DMA_Cmd(DMA2_Stream0, ENABLE); //使能DMA
	
	//GPIO配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
	
	//中断向量配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//ADC配置
	/* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA先失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
		
	/*ADC Init *********************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv=0;//这里不赋值的话 即使下面是右对齐也可能变成左对齐
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;//一个通道转换
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_480Cycles);
	
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	ADC_SoftwareStartConv(ADC1);
}


void DMA2_Stream0_IRQHandler(void)
{
    uint16_t i = 0;
//		OSIntEnter(); //进入中断 
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))  //判断DMA传输完成中断 
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);//清除标志位
 
        if(DMA_GetCurrentMemoryTarget(DMA2_Stream0))//正在访问Buffer1
        {
            //现在访问是Buufer1,所以写Buffer0.
            for(i = 1020; i < 1024; i++)
            {
                printf("%d\r\n",ADC_Buff1[i]);
            }
        }
        else
        {
            for(i = 0; i < 4; i++)
            {
 
                printf("%d\r\n",ADC_Buff0[i]);
            }
        }
 
    }
	}
