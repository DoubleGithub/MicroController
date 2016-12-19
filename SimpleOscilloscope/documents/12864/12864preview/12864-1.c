#include <STC12c5a32s2.H> 
#include<intrins.h>
sbit RS=P0^0; //���е�ָ��/����ѡ���ź�, H����, L����
sbit RW=P0^1; //���ж�дѡ���ź�, H��, Lд
sbit E=P0^2; //����ʹ�ܶ�, H��Ч, L��Ч
sbit PSB=P0^3; //��/���ӿ�ѡ��, H��,L��
sbit led=P0^4;
sbit jiakey=P1^0;
sbit jiankey=P1^1;
sbit ledkey=P1^2;
#define  LcdData P2
#define ADC_POWER 0x80
#define ADC_FLAG  0x10
#define ADC_START 0x08
#define ADC_SPEED 0x60
char ch=7;
int dati=0;
int dat[100];
int over=0;
int temp=0;
int jm=0;
int mode=0;
int delnop=0;
//////////////////////////////
void T_C_init (void){
	TMOD = 0x11; //��4λ����T/C1 [ GATE��C/T��M1��M0��GATE��C/T��M1��M0 ]
	//EA = 1;	//�ж��ܿ���

	TH0 = 0x00; //16λ�����Ĵ���T0��8λ
	TL0 = 0x00; //16λ�����Ĵ���T0��8λ��0x3CB0 = 50mS��ʱ��
	ET0 = 1; //T/C0�жϿ���
	TR0 = 1; //T/C0��������
}
void T_C0 (void) interrupt 1  using 1{ //using 1 �л��Ĵ����鵽1
	TH0 = 0x00; //16λ�����Ĵ���T0��8λ������д���ֵ��
	TL0 = 0x00; //16λ�����Ĵ���T0��8λ
	jm++;
	if(jm==10)
	{
		jm=0;
		if(ledkey==0)
			{
			led=~led;
			}
		if(jiakey==0)
			{
			if(mode==3)
			{
			}
			else
			{
			mode++;
			}
			}
		if(jiankey==0)
			{
			if(mode==0)
			{
			}
			else
			{
			mode--;
			}
			}
	}	
}
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
��LCDд���ֽ�����
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
��LCD�ж�������
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
��LCD��д��ָ�����
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
Ϊ�����߼���������õ�����������������ռ����ȡʱ��İ취
***************************************/
code unsigned int LcdMaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
/***************************************
��LCDָ������д��һ������,������ɫ�����֣�0����ף�����ʾ����1����ڣ�����ʾ��
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	Color=Color%2;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;//ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ 
	Temp=Lcd_ReadData();//�ȿն�һ��
	Temp=(unsigned int)Lcd_ReadData()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)Lcd_ReadData();//�ٶ�����8λ
	EA=1;
	if(Color==1) //���д����ɫΪ1
		Temp|=LcdMaskTab[w];//�ڴ˴����ʵ�ּ���
	else         //���д����ɫΪ0
		Temp&=~LcdMaskTab[w];//�ڴ˴����ʵ�ּ���
	EA=0;
	Lcd_WriteCmd(y+0x80);        //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ
   Lcd_WriteData(Temp>>8);//��д���8λ����д���8λ
   Lcd_WriteData(Temp&0x00ff);
	Lcd_WriteCmd(0x30);
	EA=1;
}
/*****************************************
���Lcdȫ����������ģʽModeΪ0����Ϊȫ�����Ϊ��ɫ0�����κ���ʾ��
����Ϊȫ�����Ϊ��ɫ1(ȫ�������ʾ)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//����ָ�� ��ͼ��ʾ
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				Lcd_WriteCmd(y+0x80);        //�е�ַ
				Lcd_WriteCmd(x+0x80+ii);     //�е�ַ     
				Lcd_WriteData(Temp); //д���� D15��D8 
				Lcd_WriteData(Temp); //д���� D7��D0 
				EA=1;
			}
	Lcd_WriteCmd(0x30);
}

/****************************************
LCD��ʼ��
*****************************************/
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //ѡ�����ָ�
	Lcd_WriteCmd(0x0c);       //����ʾ(���αꡢ������)
	Lcd_WriteCmd(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	Lcd_WriteCmd(0x06);       //ָ�������ݵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
}
//////////////////////////////////////
void Delay(int n)
{
int x;
while(n--)
	{
	x=5000;
	while(x--);
	}
}
void InitADC()
{
	P1ASF=0X80;
	ADC_RES=0;
	ADC_CONTR=ADC_POWER|ADC_SPEED|ADC_START|ch;
	Delay(2);
	EADC=1;
}
void adc_isr() interrupt 5 using 1
{
	ADC_CONTR&=!ADC_FLAG;
	if(over==0)
	{
		for(temp=0;temp<delnop;temp++)
	{
		_nop_();
	}
	dat[dati]=ADC_RES;
	dati++;
	if(dati>100)
	{
		dati=0;
		over=1;
	}
}
ADC_CONTR=ADC_POWER|ADC_SPEED|ADC_START|ch;
}
//////////////////////////////////////
void disp_0(char x,char y)
{
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+3,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+3,y+1,1);
Lcd_PutPixel(x+3,y+2,1);
Lcd_PutPixel(x+3,y+3,1);
}
void disp_1(char x,char y)
{
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+1,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+3,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+3,y+4,1);
}
void disp_2(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+3,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+1,1);
Lcd_PutPixel(x+3,y+2,1);
Lcd_PutPixel(x+3,y+4,1);
}
void disp_3(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+1,1);
Lcd_PutPixel(x+3,y+2,1);
Lcd_PutPixel(x+3,y+3,1);
Lcd_PutPixel(x+3,y+4,1);
}
void disp_4(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+1,1);
Lcd_PutPixel(x+3,y+2,1);
Lcd_PutPixel(x+3,y+3,1);
Lcd_PutPixel(x+3,y+4,1);
}
void disp_5(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+2,1);
Lcd_PutPixel(x+3,y+3,1);
Lcd_PutPixel(x+3,y+4,1);
}
disp_p(char x,char y)
{
Lcd_PutPixel(x+0,y+0,1);
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+1,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+1,1);
Lcd_PutPixel(x+4,y+0,1);
}
disp_k(char x,char y)
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
void disp_hz(char x,char y)
{
Lcd_PutPixel(x+0,y+0,1);
Lcd_PutPixel(x+0,y+1,1);
Lcd_PutPixel(x+0,y+2,1);
Lcd_PutPixel(x+0,y+3,1);
Lcd_PutPixel(x+0,y+4,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+2,y+0,1);
Lcd_PutPixel(x+2,y+1,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+3,1);
Lcd_PutPixel(x+2,y+4,1);
Lcd_PutPixel(x+4,y+1,1);
Lcd_PutPixel(x+4,y+3,1);
Lcd_PutPixel(x+4,y+4,1);
Lcd_PutPixel(x+5,y+1,1);
Lcd_PutPixel(x+5,y+2,1);
Lcd_PutPixel(x+5,y+4,1);
}
void disp_ledon(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+3,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+1,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+3,1);
Lcd_PutPixel(x+3,y+0,1);
Lcd_PutPixel(x+3,y+4,1);
Lcd_PutPixel(x+4,y+2,1);
}
void disp_ledoff(char x,char y)
{
Lcd_PutPixel(x+1,y+0,1);
Lcd_PutPixel(x+1,y+1,1);
Lcd_PutPixel(x+1,y+2,1);
Lcd_PutPixel(x+1,y+3,1);
Lcd_PutPixel(x+1,y+4,1);
Lcd_PutPixel(x+2,y+1,1);
Lcd_PutPixel(x+2,y+2,1);
Lcd_PutPixel(x+2,y+3,1);
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
	int x=0;
	int y=0;
	for(x=13;x<114;x++)
	{
		Lcd_PutPixel(x,0,1);
	}
	for(x=13;x<114;x++)
	{
		Lcd_PutPixel(x,52,1);
	}
	for(y=0;y<52;y++)
	{
		Lcd_PutPixel(13,y,1);
	}
	for(y=0;y<52;y++)
	{
		Lcd_PutPixel(114,y,1);
	}
Lcd_PutPixel(13,51,0);
Lcd_PutPixel(13,41,0);
Lcd_PutPixel(13,31,0);
Lcd_PutPixel(13,21,0);
Lcd_PutPixel(13,11,0);
Lcd_PutPixel(13,1,0);
Lcd_PutPixel(114,51,0);
Lcd_PutPixel(114,41,0);
Lcd_PutPixel(114,31,0);
Lcd_PutPixel(114,21,0);
Lcd_PutPixel(114,11,0);
Lcd_PutPixel(114,1,0);
Lcd_PutPixel(14,52,0);
Lcd_PutPixel(24,52,0);
Lcd_PutPixel(34,52,0);
Lcd_PutPixel(44,52,0);
Lcd_PutPixel(54,52,0);
Lcd_PutPixel(64,52,0);
Lcd_PutPixel(74,52,0);
Lcd_PutPixel(84,52,0);
Lcd_PutPixel(94,52,0);
Lcd_PutPixel(104,52,0);
disp_0(5,50);
disp_1(5,40);
disp_2(5,30);
disp_3(5,20);
disp_4(5,10);
disp_5(5,0);
disp_0(117,50);
disp_1(117,40);
disp_2(117,30);
disp_3(117,20);
disp_4(117,10);
disp_5(117,0);
disp_1(13,58);
disp_hz(18,58);
disp_1(38,58);
disp_0(43,58);
disp_hz(48,58);
disp_1(63,58);
disp_0(68,58);
disp_0(73,58);
disp_hz(78,58);
disp_1(88,58);
disp_k(93,58);
disp_hz(98,58);
}
//////////////////////////////////////
main()
{
int x=0;
int y=0;
T_C_init();
Lcd_Reset();
Lcd_Clear(0);
InitADC();
disp_bj();
EA=1;
while(1)
{
if(over)
	{
		for(x=0;x<100;x++)
		{
			for(y=1;y<52;y++)
			{
			Lcd_PutPixel(x+14,y,0);
			}
			Lcd_PutPixel(x+14,51-(dat[x]/5),1);
		}
		if(led)
		{
		clr(5,58,10,63);
		disp_ledon(5,58);
		}
		else
		{
		clr(5,58,10,63);
		disp_ledoff(5,58);
		}
	switch(mode)
	{
	case 0:
		delnop=0;
		disp_p(91,54);
		clr(66,54,71,57);
		clr(41,54,46,57);
		clr(16,54,21,57);
	break;
	case 1:
		delnop=10;
		disp_p(66,54);
		clr(91,54,96,57);
		clr(41,54,46,57);
		clr(16,54,21,57);
	break;
	case 2:
		delnop=100;
		disp_p(41,54);
		clr(91,54,96,57);
		clr(66,54,71,57);
		clr(16,54,21,57);
	break;
	case 3:
		delnop=1000;
		disp_p(16,54);
		clr(91,54,96,57);
		clr(41,54,46,57);
		clr(66,54,71,57);
	break;
	default:
	break;
	}
	over=0;
	}
}
}
