#include <reg51.h>
#include <intrins.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
/* http://ttmcu.taobao.com ��ҫ����
������Ի���:JME-2���İ�+1Tָ�����ڵ�STC��Ƭ����51�ں�STC12LE5A60S2)+33M����	  ��Ƭ��������ѹ3.3V
����Ĭ��IO���ӷ�ʽ��
�����ߣ�RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
������: DB0-DB7��������P0^0-P0^7;  DB8-DB15��������P2^0-P2^7;
�����������ӷ�ʽ��(��ʹ�ô����ɲ�����)
D_CLK-P1^7;  D_CS-P1^4;  D_DIN-P3^0;  D_OUT-P3^1;  D_PENIRQ-P3^4;
*/ 	

void WritePage(unsigned char  index)//���õ�ǰ����ҳ���ϵ�Ĭ��Ϊ0
{
	LCD_WR_REG(0x05);
	LCD_WR_DATA(index); 
}
void ShowPage(unsigned char  index)//���õ�ǰ��ʾҳ���ϵ�Ĭ��Ϊ0
{
	LCD_WR_REG(0x04);
	LCD_WR_DATA(index); 	
}

main()
{  
	unsigned char  pp=0;
	Lcd_Init();   //tft��ʼ��
	while(1)
	{   
		pp=~pp;
		WritePage(pp);
		LCD_Clear(RED);	 //��  	
	 	ShowPage(pp);
		
		pp=~pp;
		WritePage(pp);
		LCD_Clear(GREEN);	 //��
		ShowPage(pp);
		
		pp=~pp;
		WritePage(pp);
		LCD_Clear(BLUE);		 //��
	 	ShowPage(pp);
    }


}
