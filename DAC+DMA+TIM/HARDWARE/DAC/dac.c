#include "sys.h"
#include "dac.h"
#include "common.h"

extern u16 dacwave_buff0[SAMPLE_LENGTH];
extern u16 dacwave_buff1[SAMPLE_LENGTH];

//最好进行中断优先级分组  貌似不需要TIM2是没有中断服务函数的
void Dac1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	DMA_InitTypeDef  DMA_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	//时钟使能
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//使能DAC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//开启DMA1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//开启TIM2时钟
	   
	//引脚是模拟输入PA4（引脚接到dac的输出上，所以是模拟输入模式）
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	//DAC的配置（这里不知道先于DMA配置有没有影响）
	DAC_StructInit(&DAC_InitType);
	DAC_InitType.DAC_Trigger=DAC_Trigger_T2_TRGO;	//定时器2触发
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable;//不使用输出缓冲
  DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1
	
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
	
	DAC_Cmd(DAC_Channel_1, ENABLE);    //使能DAC的通道1
  DAC_DMACmd(DAC_Channel_1, ENABLE); //使能DAC通道1的DMA 
  
  //定时器配置
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//初始化结构体
	TIM_TimeBaseStructure.TIM_Period = 100-1; //周期100
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; //预分频0
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;//分频0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数 每次触发时间100*1/84M
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM2 TRGO selection 用来触发DAC*/
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
	//DMA配置
	DMA_StructInit( &DMA_InitStructure);        //DMA结构体初始化
	DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);//DAC1 12位 右对齐地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&dacwave_buff0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//从存储器到外设
	DMA_InitStructure.DMA_BufferSize = SAMPLE_LENGTH;//一次输出数据
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//宽度为半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//宽度为半字
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//优先级非常高
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         //不开启fifo
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  //突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环发送模式
	
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(uint32_t)&dacwave_buff1,DMA_Memory_1);
	DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//开启双缓冲
	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);    
        
  DMA_Cmd(DMA1_Stream5, ENABLE);
        
  DAC_Cmd(DAC_Channel_1, ENABLE);
        
  DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
}
