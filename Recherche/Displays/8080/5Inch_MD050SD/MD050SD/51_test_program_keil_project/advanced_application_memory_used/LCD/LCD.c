#include <reg51.h>
#include <intrins.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色	     
 
void LCD_Writ_Bus(char VH,char VL)   //并行数据写入函数
{
    LCD_DataPortH=VH;	
	LCD_DataPortL=VL;		
	LCD_WR=0;
	LCD_WR=1; 
}
void LCD_WR_DATA8(char VH,char VL) //发送数据-8位参数
{
    LCD_RS=1;
	LCD_Writ_Bus(VH,VL);
}  
 void LCD_WR_DATA(int da)
{
    LCD_RS=1;
	LCD_Writ_Bus(da>>8,da);
}	  
void LCD_WR_REG(int da)	 
{	
    LCD_RS=0;
	LCD_Writ_Bus(da>>8,da);
}
//设置起始地址与结束地址
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{ 
	LCD_WR_REG(0x02);//开始y
	LCD_WR_DATA(y1); 
	LCD_WR_REG(0x03);//开始x
	LCD_WR_DATA(x1);	    
   	LCD_WR_REG(0x06);//结束y
	LCD_WR_DATA(y2); 
	LCD_WR_REG(0x07);//结束x
	LCD_WR_DATA(x2);   
	LCD_WR_REG(0x0f);					 						 
}
void Lcd_Init(void)
{

	LCD_RD=1;
	LCD_WR=1;
	LCD_REST=0;
	delayms(1);	//此处保证要有1ms以上
	LCD_REST=1;	
	delayms(1);	//此处保证要有1ms以上	
	LCD_CS =0;  //打开片选使能

	LCD_WR_REG(0x01); //打开背光	
	LCD_WR_DATA(16); 


}
//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u8 VH,VL;
	u16 i,j;
	VH=Color>>8;
	VL=Color;	
	Address_set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	 LCD_WR_DATA8(VH,VL);
	    }

	  }
}





