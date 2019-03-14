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
//UI参数集
typedef struct 
{
	//SATA
	u32 STATE;//LSB第一位是光标值  第二位是setting参数  第三位是measure的参数 第四位预计是measure的框框
	//Plot
	u8 Display;//0 开启显示 1 关闭显示

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
void LCD_plotgrid(void);																											//画网格 
void LCD_plotzhou(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 * yname,u8* xname);	//
void LCD_plotwave(u16 *arr);	

	
#endif
