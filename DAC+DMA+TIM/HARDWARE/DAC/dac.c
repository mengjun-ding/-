#include "sys.h"
#include "dac.h"
#include "common.h"

extern u16 dacwave_buff0[SAMPLE_LENGTH];
extern u16 dacwave_buff1[SAMPLE_LENGTH];

//��ý����ж����ȼ�����  ò�Ʋ���ҪTIM2��û���жϷ�������
void Dac1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	DMA_InitTypeDef  DMA_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	//ʱ��ʹ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//����DMA1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����TIM2ʱ��
	   
	//������ģ������PA4�����Žӵ�dac������ϣ�������ģ������ģʽ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	//DAC�����ã����ﲻ֪������DMA������û��Ӱ�죩
	DAC_StructInit(&DAC_InitType);
	DAC_InitType.DAC_Trigger=DAC_Trigger_T2_TRGO;	//��ʱ��2����
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable;//��ʹ���������
  DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1
	
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	
	DAC_Cmd(DAC_Channel_1, ENABLE);    //ʹ��DAC��ͨ��1
  DAC_DMACmd(DAC_Channel_1, ENABLE); //ʹ��DACͨ��1��DMA 
  
  //��ʱ������
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//��ʼ���ṹ��
	TIM_TimeBaseStructure.TIM_Period = 100-1; //����100
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; //Ԥ��Ƶ0
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;//��Ƶ0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ��� ÿ�δ���ʱ��100*1/84M
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM2 TRGO selection ��������DAC*/
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
	//DMA����
	DMA_StructInit( &DMA_InitStructure);        //DMA�ṹ���ʼ��
	DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);//DAC1 12λ �Ҷ����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&dacwave_buff0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�Ӵ洢��������
	DMA_InitStructure.DMA_BufferSize = SAMPLE_LENGTH;//һ���������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���Ϊ����
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���Ϊ����
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//���ȼ��ǳ���
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         //������fifo
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  //ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ������ģʽ
	
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(uint32_t)&dacwave_buff1,DMA_Memory_1);
	DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//����˫����
	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);    
        
  DMA_Cmd(DMA1_Stream5, ENABLE);
        
  DAC_Cmd(DAC_Channel_1, ENABLE);
        
  DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
}
