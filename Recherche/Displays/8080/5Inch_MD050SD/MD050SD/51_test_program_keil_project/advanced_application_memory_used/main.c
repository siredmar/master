#include <reg51.h>
#include <intrins.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
/* http://ttmcu.taobao.com 晶耀电子
代码测试环境:JME-2核心板+1T指令周期的STC单片机（51内核STC12LE5A60S2)+33M晶振	  单片机工作电压3.3V
程序默认IO连接方式：
控制线：RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
数据线: DB0-DB7依次连接P0^0-P0^7;  DB8-DB15依次连接P2^0-P2^7;
触摸功能连接方式：(不使用触摸可不连接)
D_CLK-P1^7;  D_CS-P1^4;  D_DIN-P3^0;  D_OUT-P3^1;  D_PENIRQ-P3^4;
*/ 	

void WritePage(unsigned char  index)//设置当前操作页，上电默认为0
{
	LCD_WR_REG(0x05);
	LCD_WR_DATA(index); 
}
void ShowPage(unsigned char  index)//设置当前显示页，上电默认为0
{
	LCD_WR_REG(0x04);
	LCD_WR_DATA(index); 	
}

main()
{  
	unsigned char  pp=0;
	Lcd_Init();   //tft初始化
	while(1)
	{   
		pp=~pp;
		WritePage(pp);
		LCD_Clear(RED);	 //红  	
	 	ShowPage(pp);
		
		pp=~pp;
		WritePage(pp);
		LCD_Clear(GREEN);	 //绿
		ShowPage(pp);
		
		pp=~pp;
		WritePage(pp);
		LCD_Clear(BLUE);		 //蓝
	 	ShowPage(pp);
    }


}
