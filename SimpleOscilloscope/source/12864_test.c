/**************************************  
 * 控制器：KS0108   
 * MCU:AT89C5* ,晶体频率:12MHz   
 * 取模方式：纵向字节倒序   
 * CS1和CS2为低电平有效    
**************************************/  
#include <reg51.H>   
typedef unsigned char uchar;   
typedef unsigned int uint;     
#define LCD_PORT P1   
#define Left  1   
#define Right 2   
sbit  LCD_Busy=P1^7;   
sbit  LCD_EN = P2^0;   
sbit  LCD_RS = P2^2;   
sbit  LCD_RW = P2^1;   
sbit  LCD_RST= P2^5;   
sbit  LCD_CS2= P2^3;   
sbit  LCD_CS1= P2^4;   
   
uchar DisBuf[32],Page_Num,Clm_Num;   
   
code unsigned char Table[]=   
{   
/*--  文字:  天  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x00,0x40,0x42,0x42,0x42,0x42,0x42,0xFE,0x42,0x42,0x42,0x42,0x42,0x42,0x40,0x00,   
0x00,0x80,0x40,0x20,0x10,0x08,0x06,0x01,0x02,0x04,0x08,0x10,0x30,0x60,0x20,0x00,   
   
/*--  文字:  仙  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x00,0x80,0x60,0xF8,0x07,0x02,0xE0,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xE0,0x00,   
0x01,0x00,0x00,0x7F,0x00,0x20,0x7F,0x20,0x20,0x20,0x3F,0x20,0x20,0x20,0x7F,0x00,   
   
/*--  文字:  妹  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x08,0x88,0x7F,0x0A,0x08,0xF8,0x80,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x80,0x00,   
0x40,0x21,0x12,0x0C,0x3A,0x11,0x40,0x30,0x0C,0x03,0xFF,0x06,0x08,0x30,0x10,0x00,   
   
/*--  文字:  妹  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x08,0x88,0x7F,0x0A,0x08,0xF8,0x80,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x80,0x00,   
0x40,0x21,0x12,0x0C,0x3A,0x11,0x40,0x30,0x0C,0x03,0xFF,0x06,0x08,0x30,0x10,0x00   
 };   
   
code unsigned char Table0[]=   
{   
/*--  文字:  我  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x20,0x20,0x22,0x22,0xFE,0x21,0x21,0x20,0x20,0xFF,0x20,0x22,0xAC,0x20,0x20,0x00,   
0x04,0x04,0x42,0x82,0x7F,0x01,0x01,0x10,0x10,0x08,0x07,0x1A,0x21,0x40,0xF0,0x00,   
   
/*--  文字:  好  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x10,0x10,0xF0,0x1F,0x10,0xF0,0x80,0x82,0x82,0x82,0xF2,0x8A,0x86,0x82,0x80,0x00,   
0x80,0x43,0x22,0x14,0x0C,0x73,0x20,0x00,0x40,0x80,0x7F,0x00,0x00,0x00,0x00,0x00,   
   
/*--  文字:  想  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x08,0xC8,0x38,0xFF,0x18,0x68,0x08,0x00,0xFE,0x2A,0x2A,0x2A,0x2A,0xFE,0x00,0x00,   
0x01,0x40,0x70,0x01,0x38,0x40,0x40,0x44,0x59,0x41,0x41,0x61,0x01,0x09,0x30,0x00,   
   
/*--  文字:  你  --*/   
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/   
0x80,0x40,0xF0,0x2C,0x43,0x20,0x98,0x0F,0x0A,0xE8,0x08,0x88,0x28,0x1C,0x08,0x00,   
0x00,0x00,0x7F,0x00,0x10,0x0C,0x03,0x21,0x40,0x3F,0x00,0x00,0x03,0x1C,0x08,0x00   
};   
  
void Delay_us(unsigned int DelayTime)   
{   
   while(DelayTime) DelayTime--;   
}   
   
void Delay_x10ms(unsigned char DelayTime)   //***************10ms延时**********//   
{   
       unsigned char i,j,k;   
       for(i=0;i<DelayTime;i++)   
          for(j=0;j<10;j++)   
             for(k=0;k<120;k++) {;}   
}   
/*----------------------------------------------------------------------------   
下面这个函数用于液晶模块的忙信号检测。   
----------------------------------------------------------------------------*/    
void LCD_Check_Busy(void)   
{   
   while(1)   
   {   
      LCD_EN = 0;   
      LCD_RS = 0;   
      LCD_RW = 1;   
      LCD_PORT = 0xff;   
      LCD_EN = 1;   
      if(!LCD_Busy) 
		  break;   
   }   
   LCD_EN = 0;   
}   
   
/*----------------------------------------------------------------------------   
下面这个函数用于向液晶模块写入控制指令代码。   
函数入口参数：Command ------ 向液晶模块写入的控制指令代码 。   
              CS_Num  ------ 左右半屏的选数值   
----------------------------------------------------------------------------*/    
void LCD_WriteCommand(uchar Command, uchar CS_Num)    
{    
   //LCD_Check_Busy();   
   if(CS_Num==1)         /*选择左半屏*/   
   {   
     LCD_CS1 = 0;   
     LCD_CS2 = 1;   
   }   
   else if(CS_Num==2)    /*选择右半屏*/   
   {   
     LCD_CS1 = 1;   
     LCD_CS2 = 0;   
   }   
   LCD_EN = 0;           /* 使能信号为L */    
   LCD_RS = 0;           /* 置寄存器选择为L */    
   LCD_RW = 0;           /* 置读/写选择为L */    
   LCD_PORT = Command;   /* 将控制指令代码输出到的数据端口 */    
   LCD_EN = 1;           /* 使能信号为H */    
   Delay_us(1);          /* 延时2us */    
   LCD_EN = 0;           /* 使能信号为L */    
   LCD_CS1 = 1;          /* 置片选1为L */    
   LCD_CS2 = 1;          /* 置片选2为L */    
}     
/*----------------------------------------------------------------------------   
下面这个函数用于向液晶模块写入显示数据。   
函数入口参数：Data------ 向液晶模块写入的显示数据 。   
            CS_Num-------左右半屏的选数值   
----------------------------------------------------------------------------*/    
void LCD_WriteData(uchar Data,uchar CS_Num)    
{    
   //LCD_Check_Busy();   
   if(CS_Num==1)      /*选择左半屏*/   
   {   
     LCD_CS1 = 0;   
     LCD_CS2 = 1;   
   }   
   else if(CS_Num==2)  /*选择右半屏*/   
   {   
     LCD_CS1 = 1;   
     LCD_CS2 = 0;   
   }   
   LCD_EN = 0;   
   LCD_RS = 1;       /* 置寄存器选择为H */    
   LCD_RW = 0;       /* 置读/写选择为L */     
   LCD_PORT = Data;  /* 将显示数据输出到的数据端口 */    
   LCD_EN = 1;       /* 使能信号为H */    
   Delay_us(1);      /* 延时2us */    
   LCD_EN = 0;       /* 使能信号为L */    
   LCD_CS1= 1;       /* 置片选1为L */    
   LCD_CS2= 1;       /* 置片选2为L */    
}       
/*---------------------------------------------------------------------------  
*下面这个函数用于向液晶模块左右半屏写入汉字显示数据。  
*函数入口参数：DisBuf-----向液晶模块写入的汉字显示数据  
            Side_Num------液晶模块左右半屏的选择数  
            Page_Num------开始显示页数（0----7），共八页  
             Clm_Num------开始显示列数 （16的倍数），共64列  
            Word_Num------每页显示的汉字个数（最多为4个）  
---------------------------------------------------------------------------*/   
void LCD_Play(uchar Page_Num,uchar Clm_Num,uchar Word_Num,uchar *DisBuf,uchar Side_Num)   
{   
   uchar i,k,COM;   
                 
   COM=0xB8+Page_Num;   
   LCD_WriteCommand(COM,Side_Num);       
   COM=0x40+Clm_Num;   
   LCD_WriteCommand(COM,Side_Num);   
   for(i=0;i<Word_Num;i++)   
   {            
     for(k=0;k<16;k++)   
     {   
        LCD_WriteData(DisBuf[k+32*i],Side_Num);   
     }   
   }    
   Page_Num++;   
   COM=0xB8+Page_Num;   
   LCD_WriteCommand(COM,Side_Num);       
   COM=0x40+Clm_Num;   
   LCD_WriteCommand(COM,Side_Num);   
   for(i=0;i<Word_Num;i++)   
   {   
      for(k=16;k<32;k++)   
      {   
         LCD_WriteData(DisBuf[k+32*i],Side_Num);   
      }   
   }   
   Delay_us(2);   
 }   
   
   
/*---------------------------------------------------------------------------  
下面这个函数用于液晶模块的清屏。  
---------------------------------------------------------------------------*/   
void LCD_CLR(uchar Side_Num)   
{   
   uchar i,k,COM;      
   for(i=0;i<8;i++)                   
   {   
      COM=0xB8+i;   
      LCD_WriteCommand(COM,Side_Num);       
      COM=0x40;   
      LCD_WriteCommand(COM,Side_Num);     
            
      for(k=0;k<64;k++)   
      {   
         LCD_WriteData(0x00,Side_Num);   
      }   
   }   
   Delay_us(2);   
}   
/*---------------------------------------------------------------------------  
下面这个函数用于向液晶模块初始化。  
如果没有写完一个半屏的数据就要充分结合此函数来达到要显示的效果  
---------------------------------------------------------------------------*/   
void LCD_Initial(void)    
{      
    LCD_RST = 0;    /* 置复位信号为低电平 */    
    Delay_us(2);     
    LCD_RST = 1;    /* 置复位信号为高电平 */     
    LCD_WriteCommand(0x3F,Left);    /*显示开*/   
    LCD_WriteCommand(0x3F,Right);   
    LCD_WriteCommand(0xC0,Left);    /*起始行*/   
    LCD_WriteCommand(0xC0,Right);                
    LCD_WriteCommand(0xB8,Left);    /*起始页*/   
    LCD_WriteCommand(0xB8,Right);     
    LCD_WriteCommand(0x40,Left);    /*起始列*/   
    LCD_WriteCommand(0x40,Right);    
}    
/*---------------------------------------------------------------------------  
下面这个函数为主函数，根据程序要实现的功能写相应的应用程序  
---------------------------------------------------------------------------*/   
void main(void)    
{    
    LCD_Initial(); //液晶屏初始化    
    LCD_CLR(Left);   //清左边屏
    LCD_CLR(Right);//清右边屏   
    LCD_Play(3,0,4,Table,Left);//显示table数组数据,并显示在左边   
    LCD_Play(3,0,4,Table0,Right); //显示table0数组，并显示在右边  
    Delay_x10ms(250);   //延时
    Delay_x10ms(250);   
    Delay_x10ms(250);   
    Delay_x10ms(250);   
 }    