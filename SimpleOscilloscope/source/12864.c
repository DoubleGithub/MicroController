//#include <STC12c5a32s2.H>
#include <STC12c5a60s2.H> 
#include<intrins.h>
sbit RS=P0^0; //并行的指令/数据选择信号, H数据, L命令
sbit RW=P0^1; //并行读写选择信号, H读, L写
sbit E=P0^2; //并行使能端, H有效, L无效
sbit PSB=P0^3; //并/串接口选择, H并,L串
sbit led=P0^4;
sbit fkey=P1^0;
sbit vkey=P1^1;
sbit ledkey=P1^2;
#define  LcdData P2
unsigned char dati=0;
unsigned char dat[101];
unsigned char dat0[101];
unsigned char over=0;
unsigned int temp=0;
unsigned char mode=0;
unsigned char vmode=0;
unsigned int delnop=0;
unsigned char vd=4;
unsigned char cha=0;
//////////////////////////////////////
unsigned char Lcd_CheckBusy(void)
{
    unsigned char Busy;
	 LcdData=0xff;
    RS=0;
    RW=1;
    E=1;
    _nop_(); 
    Busy=LcdData&0x80;
    E=0;
    return Busy;
}
/*********************************
向LCD写入字节数据
**********************************/
void Lcd_WriteData(unsigned char Data)
{  
	while(Lcd_CheckBusy());
	RS=1;
	RW=0;
	E=0;
	_nop_();  
	_nop_(); 
	LcdData=Data;
	E=1;
	_nop_();
	_nop_();
	E=0;
}
/***********************************
从LCD中读出数据
************************************/
unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;
 	RS=1;
	RW=1;
	E=1;
	_nop_();
   	Temp=LcdData;
   	E=0;
   	return Temp;
}
/*************************************
想LCD中写入指令代码
**************************************/
void Lcd_WriteCmd(unsigned char CmdCode)
{  
	while(Lcd_CheckBusy());
   	RS=0;
   	RW=0;
   	E=0;
   	_nop_();  
	_nop_();
   	LcdData=CmdCode;
   	_nop_(); 
	_nop_();
   	E=1;
   	_nop_();  
	_nop_();
   	E=0;
}
/**************************************
为加速逻辑运算而设置的掩码表，这是以牺牲空间而换取时间的办法
***************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
/***************************************
向LCD指定坐标写入一个象素,象素颜色有两种，0代表白（无显示），1代表黑（有显示）
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	Color=Color%2;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;//确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	//EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //行地址
	Lcd_WriteCmd(x+z);     //列地址 
	Temp=Lcd_ReadData();//先空读一次
	Temp=(unsigned int)Lcd_ReadData()<<8;//再读出高8位
	Temp|=(unsigned int)Lcd_ReadData();//再读出低8位
	//EA=1;
	if(Color==1) //如果写入颜色为1
		Temp|=LcdMaskTab[w];//在此处查表实现加速
	else         //如果写入颜色为0
		Temp&=~LcdMaskTab[w];//在此处查表实现加速
	//EA=0;
	Lcd_WriteCmd(y+0x80);        //行地址
	Lcd_WriteCmd(x+z);     //列地址
	Lcd_WriteData(Temp>>8);//先写入高8位，再写入低8位
	Lcd_WriteData(Temp&0x00ff);
	Lcd_WriteCmd(0x30);
	//EA=1;
}
/*****************************************
清除Lcd全屏，如果清除模式Mode为0，则为全屏清除为颜色0（无任何显示）
否则为全屏清除为颜色1(全屏填充显示)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//扩充指令 绘图显示
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				//EA=0;
				Lcd_WriteCmd(y+0x80);        //行地址
				Lcd_WriteCmd(x+0x80+ii);     //列地址     
				Lcd_WriteData(Temp); //写数据 D15－D8 
				Lcd_WriteData(Temp); //写数据 D7－D0 
				//EA=1;
			}
	Lcd_WriteCmd(0x30);
}
/****************************************
LCD初始化
*****************************************/
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //选择基本指令集
	Lcd_WriteCmd(0x0c);       //开显示(无游标、不反白)
	Lcd_WriteCmd(0x01);       //清除显示，并且设定地址指针为00H
	Lcd_WriteCmd(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
}
//////////////////////////////////////
void InitADC()
{
	P1ASF=0X80;
	ADC_RES=0;
	ADC_CONTR=0xef;
	EADC=1;
}
void adc_isr() interrupt 5 using 1
{
	ADC_CONTR=0xef;
	if(over==0)
	{
		temp=delnop;
		while(temp)
		{
			temp--;
		}
		dat[dati]=ADC_RES;
		dati++;
		if(dati>101)
		{
			dati=0;
			over=1;
		}
	}
}
//////////////////////////////////////
void disp_0(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+1,y+0,1);
	Lcd_PutPixel(x+1,y+4,1);
	Lcd_PutPixel(x+2,y+1,1);
	Lcd_PutPixel(x+2,y+2,1);
	Lcd_PutPixel(x+2,y+3,1);
}
void disp_1(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+0,1);
	Lcd_PutPixel(x+1,y+1,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+3,1);
	Lcd_PutPixel(x+1,y+4,1);
	Lcd_PutPixel(x+2,y+4,1);
}
void disp_2(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+0,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+4,1);
	Lcd_PutPixel(x+2,y+0,1);
	Lcd_PutPixel(x+2,y+1,1);
	Lcd_PutPixel(x+2,y+2,1);
	Lcd_PutPixel(x+2,y+4,1);
}
void disp_4(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+2,y+0,1);
	Lcd_PutPixel(x+2,y+1,1);
	Lcd_PutPixel(x+2,y+2,1);
	Lcd_PutPixel(x+2,y+3,1);
	Lcd_PutPixel(x+2,y+4,1);
}
void disp_5(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+0,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+4,1);
	Lcd_PutPixel(x+2,y+0,1);
	Lcd_PutPixel(x+2,y+2,1);
	Lcd_PutPixel(x+2,y+3,1);
	Lcd_PutPixel(x+2,y+4,1);
}
void disp_dian(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+4,1);
}
void disp_k(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+2,y+1,1);
	Lcd_PutPixel(x+2,y+3,1);
	Lcd_PutPixel(x+3,y+0,1);
	Lcd_PutPixel(x+3,y+4,1);
}
void disp_hz(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+2,y+2,1);
	Lcd_PutPixel(x+3,y+0,1);
	Lcd_PutPixel(x+3,y+1,1);
	Lcd_PutPixel(x+3,y+2,1);
	Lcd_PutPixel(x+3,y+3,1);
	Lcd_PutPixel(x+3,y+4,1);
	Lcd_PutPixel(x+5,y+1,1);
	Lcd_PutPixel(x+5,y+3,1);
	Lcd_PutPixel(x+5,y+4,1);
	Lcd_PutPixel(x+6,y+1,1);
	Lcd_PutPixel(x+6,y+2,1);
	Lcd_PutPixel(x+6,y+4,1);
}
void disp_vd(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+3,1);
	Lcd_PutPixel(x+2,y+4,1);
	Lcd_PutPixel(x+3,y+2,1);
	Lcd_PutPixel(x+3,y+3,1);
	Lcd_PutPixel(x+4,y+0,1);
	Lcd_PutPixel(x+4,y+1,1);
	Lcd_PutPixel(x+5,y+3,1);
	Lcd_PutPixel(x+5,y+4,1);
	Lcd_PutPixel(x+6,y+2,1);
	Lcd_PutPixel(x+7,y+0,1);
	Lcd_PutPixel(x+7,y+1,1);
	Lcd_PutPixel(x+8,y+2,1);
	Lcd_PutPixel(x+8,y+3,1);
	Lcd_PutPixel(x+8,y+4,1);
	Lcd_PutPixel(x+9,y+2,1);
	Lcd_PutPixel(x+9,y+4,1);
	Lcd_PutPixel(x+10,y+0,1);
	Lcd_PutPixel(x+10,y+1,1);
	Lcd_PutPixel(x+10,y+2,1);
	Lcd_PutPixel(x+10,y+3,1);
	Lcd_PutPixel(x+10,y+4,1);
}
void disp_ledon(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+1,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+3,1);
	Lcd_PutPixel(x+2,y+0,1);
	Lcd_PutPixel(x+2,y+4,1);
	Lcd_PutPixel(x+3,y+2,1);
}
void disp_ledoff(unsigned char x,unsigned char y)
{
	Lcd_PutPixel(x+0,y+0,1);
	Lcd_PutPixel(x+0,y+1,1);
	Lcd_PutPixel(x+0,y+2,1);
	Lcd_PutPixel(x+0,y+3,1);
	Lcd_PutPixel(x+0,y+4,1);
	Lcd_PutPixel(x+1,y+1,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_PutPixel(x+1,y+3,1);
}
void clr(unsigned char starx,unsigned char stary,unsigned char endx,unsigned char endy)
{
	char x=0;
	char y=0;
	for(x=starx;x<endx;x++)
	{
		for(y=stary;y<endy;y++)
		{
			Lcd_PutPixel(x,y,0);
		}
	}
}
void disp_bj(void)
{
	unsigned char x=0;
	unsigned char y=0;
	for(y=0;y<63;y=y+2)
	{
		Lcd_PutPixel(51,y,1);
	}
	for(y=0;y<63;y=y+2)
	{
		Lcd_PutPixel(77,y,1);
	}
	for(y=0;y<63;y=y+2)
	{
		Lcd_PutPixel(101,y,1);
	}
	for(x=27;x<127;x=x+2)
	{
		Lcd_PutPixel(x,0,1);
	}
		for(x=27;x<127;x=x+2)
	{
		Lcd_PutPixel(x,15,1);
	}
	for(x=27;x<127;x=x+2)
	{
		Lcd_PutPixel(x,31,1);
	}
	for(x=27;x<127;x=x+2)
	{
		Lcd_PutPixel(x,47,1);
	}
	for(x=27;x<127;x=x+2)
	{
		Lcd_PutPixel(x,63,1);
	}
	for(y=0;y<64;y++)
	{
		Lcd_PutPixel(26,y,1);
		Lcd_PutPixel(127,y,1);
	}
}
void line(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char color)
{
	int i,dx,dy,e,x,y;
	Lcd_PutPixel(x0,y0,color);
	Lcd_PutPixel(x1,y1,color);
	dx=x1-x0;
	dy=y1-y0;
	x=x0;
	y=y0;
	if(dx>0&&dy>0)
	{
		if(dx>dy)
		{
			e=-dx;
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x++;
				e=e+2*dy;
				if(e>=0)
				{
					y++;
					e=e-2*dx;
				}
			}
		}
		else
		{
			e=-dy;
			x=x0;
			y=y0;
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y++;
				e=e+2*dx;
				if(e>=0)
				{
					x++;
					e=e-2*dy;
				}
			}
		}
	}
	if(dx<0&&dy<0)
	{
		dx=x0-x1;
		dy=y0-y1;
		if(dx>dy)
		{
			e=-dx;
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x--;
				e=e+2*dy;
				if(e>=0)
				{
					y--;
					e=e-2*dx;
				}
			}
		}
		else
		{
			e=-dy;
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y--;
				e=e+2*dx;
				if(e>=0)
				{
					x--;
					e=e-2*dy;
				}
			}
		}
	}
	if(dx>0&&dy<0)
	{
		dy=y0-y1;
		if(dx>dy)
		{
			e=-dx;
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x++;
				e=e+2*dy;
				if(e>=0)
				{
					y--;
					e=e-2*dx;
				}
			}
		}
		else
		{
			e=-dy;
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y--;
				e=e+2*dx;
				if(e>=0)
				{
					x++;
					e=e-2*dy;
				}
			}
		}
	}
	if(dx<0&&dy>0)
	{
		dx=x0-x1;
		if(dx>dy)
		{
			e=-dx;
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x--;
				e=e+2*dy;
			if(e>=0)
				{
					y++;
					e=e-2*dx;
				}
			}
		}
		else
		{
			e=-dy;
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y++;
				e=e+2*dx;
			if(e>=0)
				{
					x--;
					e=e-2*dy;
				}
			}
		}
	}
	if(dx!=0&&dy==0)
	{
		if(dx>0)
		{
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x++;
			}
		}
		else
		{
			dx=x0-x1;
			for(i=0;i<dx;i++)
			{
				Lcd_PutPixel(x,y,color);
				x--;
			}
		}
	}
	if(dx==0&&dy!=0)
	{
		if(dy>0)
		{
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y++;
			}
		}
		else
		{
			dy=y0-y1;
			for(i=0;i<dy;i++)
			{
				Lcd_PutPixel(x,y,color);
				y--;
			}
		}
	}
}
void disp_ware()
{
	unsigned char x;
	for(x=0;x<100;x++)
	{
		line(x+27,(dat0[x]/vd)-cha,x+1+27,(dat0[x+1]/vd)-cha,0);
		line(x+27,(dat[x]/vd)-cha,x+1+27,(dat[x+1]/vd)-cha,1);
		dat0[x]=dat[x];
	}
	dat0[101]=dat[101];
	disp_bj();
}
void disp_ware50()
{
	unsigned char x;
	for(x=0;x<50;x++)
	{
		line(x*2+27,(dat0[x]/vd)-cha,(x+1)*2+27,(dat0[x+1]/vd)-cha,0);
		line(x*2+27,(dat[x]/vd)-cha,(x+1)*2+27,(dat[x+1]/vd)-cha,1);
		dat0[x]=dat[x];
	}
	disp_bj();
}
//////////////////////////////////////
void main()
{
	Lcd_Reset();
	Lcd_Clear(0);
	InitADC();

	disp_4(0,10);
	disp_k(4,10);
	disp_hz(9,10);

	disp_1(0,30);
	disp_vd(4,30);

	disp_ledon(0,50);

	EA=1;
	while(1)
	{
		if(over)
		{
			if(mode==0)
			{
				disp_ware50();
			}
			else
			{
				disp_ware();
			}
			if(ledkey==0)
			{
				led=~led;
				if(led)
				{
					clr(0,50,5,55);
					disp_ledon(0,50);
				}
				else
				{
					clr(0,50,5,55);
					disp_ledoff(0,50);
				}
			}
			if(fkey==0)
			{
				if(mode<4)
				{
					mode++;
				}
				else
				{
					mode=0;
				}
				switch(mode)
				{
					case 0://4khz
						delnop=1;
						Lcd_Clear(0);
						disp_bj();
						clr(0,10,20,15);
						disp_4(0,10);
						disp_k(4,10);
						disp_hz(9,10);
						break;
					case 1://2khz
						delnop=1;
						clr(0,10,20,15);
						disp_2(0,10);
						disp_k(4,10);
						disp_hz(9,10);
						break;
					case 2://200hz
						delnop=40;
						clr(0,10,20,15);
						disp_2(0,10);
						disp_0(4,10);
						disp_0(8,10);
						disp_hz(12,10);
						break;
					case 3://20hz
						delnop=440;
						clr(0,10,20,15);
						disp_2(0,10);
						disp_0(4,10);
						disp_hz(8,10);
						break;
					case 4://2hz
						delnop=4440;
						clr(0,10,20,15);
						disp_2(0,10);
						disp_hz(4,10);
						break;
					default:
						break;
				}
			}
			if(vkey==0)
			{
				if(vmode<2)
				{
					vmode++;
				}
				else
				{
					vmode=0;
				}
				switch(vmode)
				{
					case 0://1v/d
						vd=4;
						cha=0;
						clr(0,30,26,35);
						disp_1(0,30);
						disp_vd(4,30);
						break;
					case 1://0.5v/d
						vd=2;
						cha=32;
						clr(0,30,26,35);
						disp_0(0,30);
						disp_dian(4,30);
						disp_5(6,30);
						disp_vd(10,30);
						break;
					case 2://0.25v/d
						vd=1;
						cha=96;
						clr(0,30,26,35);
						disp_0(0,30);
						disp_dian(4,30);
						disp_2(6,30);
						disp_5(10,30);
						disp_vd(14,30);
						break;
					default:
						break;
				}
			}
			over=0;
		}
	}
}
