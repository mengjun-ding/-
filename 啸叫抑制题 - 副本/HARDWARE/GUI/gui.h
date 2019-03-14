#ifndef __GUI_H
#define __GUI_H

#include "sys.h"
#include "lcd.h"
#include "usart.h"

#define ARROW

#define COLS 2
//GUI_PLOT
#define XSTA 80
#define YSTA 20
#define XEND 380
#define YEND 220
#define YZERO (YEND+YSTA)/2

#define WAVE_LENGTH 256
//UI������
typedef struct 
{
	//SATA
	u32 STATE;//LSB��һλ�ǹ��ֵ  �ڶ�λ��setting����  ����λ��measure�Ĳ��� ����λԤ����measure�Ŀ��
	//Plot
	u8 Display;//0 ������ʾ 1 �ر���ʾ

	u16 FFT_point_num;
	float FFT_wave[WAVE_LENGTH];
	float FFT_result[WAVE_LENGTH];
	//Cursor
	u8 IsCrusor;
	s16 dis_x1;
	s16 dis_y1;
	s16 dis_x2;
	s16 dis_y2;
	s16 dis_dx;
	s16 dis_dy;
}GUI_parameter;

void LCDGUI_Init(void);
void LCD_plotgrid(void);																											//������ 
void LCD_plotzhou(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 * yname,u8* xname);	//
void LCD_plotwave(u16 *arr);	

	
#endif
