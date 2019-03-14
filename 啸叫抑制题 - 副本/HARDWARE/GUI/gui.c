#include "gui.h"
#include "lcd.h"

void LCDGUI_Init()
{
	LCD_Clear(BLACK);
	POINT_COLOR=YELLOW;
	LCD_plotgrid();
}
void LCD_plotzhou(u16 xsta,u16 ysta,u16 xend,u16 yend,u8 * yname,u8* xname)
{
	int i;
	
	#ifdef ARROW
	LCD_DrawLine(xsta,ysta,xsta,yend);//纵轴
	LCD_DrawLine(xsta-9,ysta+9,xsta,ysta);
	LCD_DrawLine(xsta,ysta,xsta+9,ysta+9);//纵轴的箭头
	LCD_DrawLine(xsta,yend,xend,yend);//横轴
	LCD_DrawLine(xend-9,yend-9,xend,yend);
	LCD_DrawLine(xend,yend,xend-9,yend);//纵轴的箭头
	#endif
//	POINT_COLOR=DARKYELLOW;//灰黄
//	for(i=0;i<(xend-xsta)/10+1;i++)
//	{
//		LCD_DrawLine(xsta+i*10,ysta,xsta+i*10,yend);//纵轴浅线
//	}
//	for(i=0;i<(yend-ysta)/10+1;i++)
//	{
//		LCD_DrawLine(xsta,ysta+i*10,xend,ysta+i*10);//横轴浅线
//	}
//	POINT_COLOR=YELLOW;//恢复黄颜色
//	for(i=0;i<(xend-xsta)/50+1;i++)
//	{
//		LCD_DrawLine(xsta+i*50,ysta,xsta+i*50,yend);//纵轴深线
//	}
//	for(i=0;i<(yend-ysta)/50+1;i++)
//	{
//		LCD_DrawLine(xsta,ysta+i*50,xend,ysta+i*50);//横轴深线
//	}
}

void LCD_plotgrid(void)
{
	LCD_plotzhou(XSTA,YSTA,XEND,YEND,"y axis","x axis");
}
void LCD_clcwave(u16 rows)
{
	LCD_DrawLine_Color(XSTA+rows,YSTA+1,XSTA+rows,YEND-1,BLACK);
}

void LCD_plotwave(u16* arr)
{
	u16 j;
	u16 temp;
	temp = POINT_COLOR;
	POINT_COLOR = LIGHTBLUE;
	for(j=1;j<WAVE_LENGTH-1;j++)
	{
		if(arr[j]>200) arr[j]=200;
		if(arr[j+1]>200) arr[j+1]=200;//限幅
		LCD_clcwave(j);
		//LCD_Fast_DrawPoint(XSTA+arr[j][0],YZERO-arr[j][1],WHITE);//蓝色点
		LCD_DrawLine_Color(XSTA+j, YEND-arr[j], XSTA+j, YEND-arr[j+1],LIGHTBLUE);							//画一条竖线
	}
	POINT_COLOR = temp;
}
