#include "sys.h"
#include "common.h"
#include "adc.h"
#include "usart.h"
#include "fft.h"

extern u16 ADC_Buff0[SAMPLE_LENGTH];//4^5
extern u16 ADC_Buff1[SAMPLE_LENGTH];//4^5

extern u16 dacwave_buff0[SAMPLE_LENGTH];
extern u16 dacwave_buff1[SAMPLE_LENGTH];

extern float fft_outputbuff[SAMPLE_LENGTH*2];	//FFT��������
extern float fft_outputmag[SAMPLE_LENGTH];	//FFT�������ֵ
extern float fft_inputbuff[SAMPLE_LENGTH];	//FFT�������

extern float dsp_signal[SAMPLE_LENGTH];

//��øĳɶ�ʱ�������ĵ���ת�� ��ƥ��DAC��ת�� ʵ��ת����ͬ��
void Adc_Init(void)
{
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;//���ö�ʱ��������� ò��ûʲô��
  DMA_InitTypeDef DMA_InitStructure;    
  NVIC_InitTypeDef NVIC_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;    
	


	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd ( RCC_APB1Periph_TIM2 , ENABLE );
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ�� 
	
	//���ö�ʱ��2
	TIM_DeInit ( TIM2 );
	//��ʱ������
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);//��ʼ���ṹ��
	TIM_TimeBaseStructure.TIM_Period = 2100-1; //����100
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; //Ԥ��Ƶ0
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;//��Ƶ0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ��� ÿ�δ���ʱ��2100*1/84M  40kƵ��
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM2 TRGO selection ��������DAC*/
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM2, ENABLE);
	
	
	//DMA2 channel0����
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;    //ADC1��channel0
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Buff0[0];//���������
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);//adc��DR�Ĵ�������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//�����赽�洢��
	DMA_InitStructure.DMA_BufferSize = SAMPLE_LENGTH;                   //����SAMPLE_LENGTH��ֵ
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          //�洢����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//������ִ��� һ��2���ֽ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //�洢��Ҳ��2���ֽ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//��ʹ��FIFO      
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_DoubleBufferModeConfig(DMA2_Stream0,(uint32_t)&ADC_Buff1,DMA_Memory_1);//���ADC_Buff0��memory0  ADC_Buff1��memory1 
	DMA_DoubleBufferModeCmd(DMA2_Stream0,ENABLE);//����˫����
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);//��������
	
 //����DMA�ж�
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC); //����жϱ�־ 
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE); //��������ж� ��������жϣ���Ҫ��                              
	DMA_Cmd(DMA2_Stream0, ENABLE); //ʹ��DMA
	
	//GPIO����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
	
	//�ж���������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//ADC����
	/* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA��ʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
		
	/*ADC Init *********************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//��ʹ������ת��
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_TRGO;//tim2����//���ﲻ��ֵ�Ļ� ��ʹ�������Ҷ���Ҳ���ܱ�������
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;//һ��ͨ��ת��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_480Cycles);
	
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

//	ADC_SoftwareStartConv(ADC1);
}


void DMA2_Stream0_IRQHandler(void)
{
    uint16_t i = 0;
//		OSIntEnter(); //�����ж� 
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))  //�ж�DMA��������ж� 
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);//�����־λ
 
        if(DMA_GetCurrentMemoryTarget(DMA2_Stream0))//���ڷ���Buffer1
        {
				//���ڷ�����Buufer1,����cpu����Buffer0.
//					DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE); //��������ж� ��������жϣ���Ҫ��
					for(i=0;i<SAMPLE_LENGTH;i++)//��ֵ��ȫ������string.h
					{
						fft_inputbuff[i]=ADC_Buff0[i]/1.0;
					}
					fft_1024(fft_outputmag,fft_outputbuff,fft_inputbuff);
					ifft_fliter_1024(fft_outputmag,dsp_signal,fft_outputbuff);

					for(i=0;i<SAMPLE_LENGTH;i++)//��û����������ĺ�����
					{
						dacwave_buff0[i]=(u16)dsp_signal[i];
					}
					
        }
        else
        {
					for(i=0;i<SAMPLE_LENGTH;i++)//��ֵ��ȫ������string.h
					{
						fft_inputbuff[i]=ADC_Buff1[i]/1.0;
					}
					fft_1024(fft_outputmag,fft_outputbuff,fft_inputbuff);
					ifft_fliter_1024(fft_outputmag,dsp_signal,fft_outputbuff);
					
					for(i=0;i<SAMPLE_LENGTH;i++)//��û����������ĺ�����
					{
						dacwave_buff1[i]=(u16)dsp_signal[i];
					}
        }
    }
	}
