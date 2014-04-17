#include <reg51.h>
#include <intrins.h>
#include <sys\sys.h>
#include <lcd\lcd.h>
u16 BACK_COLOR, POINT_COLOR;   //����ɫ������ɫ	     
 
void LCD_Writ_Bus(char VH,char VL)   //��������д�뺯��
{
    LCD_DataPortH=VH;	
	LCD_DataPortL=VL;		
	LCD_WR=0;
	LCD_WR=1; 
}
void LCD_WR_DATA8(char VH,char VL) //��������-8λ����
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
//������ʼ��ַ�������ַ
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{ 
	LCD_WR_REG(0x02);//��ʼy
	LCD_WR_DATA(y1); 
	LCD_WR_REG(0x03);//��ʼx
	LCD_WR_DATA(x1);	    
   	LCD_WR_REG(0x06);//����y
	LCD_WR_DATA(y2); 
	LCD_WR_REG(0x07);//����x
	LCD_WR_DATA(x2);   
	LCD_WR_REG(0x0f);					 						 
}
void Lcd_Init(void)
{

	LCD_RD=1;
	LCD_WR=1;
	LCD_REST=0;
	delayms(1);	//�˴���֤Ҫ��1ms����
	LCD_REST=1;	
	delayms(1);	//�˴���֤Ҫ��1ms����	
	LCD_CS =0;  //��Ƭѡʹ��

	LCD_WR_REG(0x01); //�򿪱���	
	LCD_WR_DATA(16); 


}
//��������
//Color:Ҫ���������ɫ
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





