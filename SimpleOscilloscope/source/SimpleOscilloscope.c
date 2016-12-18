#include <reg51.h>
#include <intrins.h>
//#include <ADC0808.h>
sbit RS = P2 ^ 2; // 数据\指令 选择
sbit RW = P2 ^ 1; // 读\写 选择
sbit E = P2 ^ 0; // 读\写使能
sbit CS1 = P2 ^ 4; // 片选1
sbit CS2 = P2 ^ 3; // 片选2
sbit busy = P1 ^ 7;
sbit jia = P3 ^ 7;
sbit jian = P3 ^ 3;
sbit jia1 = P3 ^ 0;
sbit jian1 = P3 ^ 1;
sbit START = P3 ^ 4;
sbit EOC = P3 ^ 5;
sbit OE = P3 ^ 6;

unsigned int i;
unsigned char ye, lei, shu;
unsigned char a[94];
#define uint unsigned int
#define uchar unsigned char
uint AD = 0;
char code t[] = {
0x00,0x20,0x00,0x3C,0xC0,0x23,0x38,0x02,0xE0,0x02,0x00,0x27,0x00,0x38,0x00,0x20,/*"A",0*/

0x08,0x20,0xF8,0x3F,0xF8,0x00,0x00,0x3F,0xF8,0x00,0xF8,0x3F,0x08,0x20,0x00,0x00,/*"M",1*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*" ",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*" ",1*/

0x08,0x20,0xF8,0x3F,0x88,0x20,0x88,0x00,0xE8,0x03,0x08,0x00,0x10,0x00,0x00,0x00,/*"F",0*/

0x08,0x20,0xF8,0x3F,0x88,0x20,0x88,0x00,0x88,0x03,0x88,0x0C,0x70,0x30,0x00,0x20,/*"R",1*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*" ",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*" ",1*/

};

void delay_50us(uint t)
{
  uchar j;
  for (; t > 0; t--)
    for (j = 19; j > 0; j--);
}
void adc(){
	START = 0 ;
	OE = 0 ;
	START = 1 ;
	delay_50us(1);
	START =0 ;
	while(!EOC);
	OE = 1 ;
	AD = P0;
	//return AD;
}

void checkstate()
{
  //bit dat;
  RS = 0;
  RW = 1;
  //P1=0x00;
  do
  {
    E = 1;
    E = 0;
    //仅当第7位为0时才可操作(判别busy信号)
  }
  while (busy == 1);
}
void writecommand(unsigned char command) 	//写命令
{
  checkstate();
  RS = 0;
  RW = 0;
  P1 = command;
  E = 1;
  E = 0;
}
void writedate(unsigned char date)		//写数据
{
  checkstate();
  RS = 1;
  RW = 0;
  P1 = date;
  E = 1;
  E = 0;
}

void pianxuan(unsigned int i)
{
  if (i == 1)
    CS1 = 0, CS2 = 1;
  if (i == 2)
    CS1 = 1, CS2 = 0;
  if (i == 3)
    CS1 = 0, CS2 = 0;
}
void cleanscreen()						 //清屏
{
  unsigned char page, i;
  for (page = 0xb8; page < 0xc0; page++)
  {
    writecommand(page);
    writecommand(0x40);
    for (i = 0; i < 64; i++)
      writedate(0x00);
  }

}
void init(void)   //开显示，起始行0行
{
  writecommand(0xc0);
  writecommand(0x3f);
}

void shuju()
{
  pianxuan(i);
  writecommand(ye);
  writecommand(lei);
  writedate(shu);
}
void qing()
{
  uchar i;
  for (i = 0xb8; i <= 0xbf; i++)
  {
    ye = i;
    shu = 0x00;
    shuju();
  }
}
void main()
{


  uint r, j, g = 0, q, dianya = 1, k;
  uchar U,l;
  pianxuan(3);
  cleanscreen();
  init();
  pianxuan(2);

  l = 0xb8;
  for (k = 0; k < 4; k++, l = l + 0x02)
  {
    ye = l;
    //lei = 0x70;
		lei = 96;
    for (r = 0; r < 16; r++)
    {
      shu = t[2 * r  + 32 * k];
      shuju();
      lei++;
    }
    ye = l + 0x01;
    //lei = 0x70;
		lei = 96;
    for (r = 0; r < 16; r++)
    {
      shu = t[2 * r + 1 + 32 * k];
      shuju();
      lei++;
    }
  }


  while (1)
  {
    uchar d1, d2, d3, d4, d5;
aaa: do
    {
      adc();
    } 
	while ((AD != 128) && (AD != 129) && (AD != 130));
    U = AD;
    adc();
    if (U > AD)
      goto aaa;
    while (jia == 0)
    {
      while (jia == 0);
      g = g + 1;
    }
    while (jian == 0)
    {
      while (jian == 0);
      if (g != 0) g = g - 1;
    }
    while (jia1 == 0)
    {
      while (jia1 == 0);
      dianya = dianya + 1;
    }
    while (jian1 == 0)
    {
      while (jian1 == 0);
      if (dianya != 1) 
		  dianya = dianya - 1;
    }
    for (j = 0; j < 94; j++) //AD采样
    {
      adc();
      a[j] = AD;
      delay_50us(g);
    }
    lei = 0x40;
    for (r = 0, j = 0; r < 94; r++, j++)
    {
      if (j < 64) i = 1;
      if (j == 64) lei = 0x40;
      if (j >= 64)i = 2;
      qing();                //清掉之前的内容
      AD = a[j] * 0.196 / dianya; //计算在12864的电压值
      if (AD <= 7)       ye = 0xbf, shu = (0x80 >> AD);
      else if (AD <= 15) ye = 0xbe, shu = 0x80 >> (AD - 8);
      else if (AD <= 23) ye = 0xbd, shu = 0x80 >> (AD - 16);
      else if (AD <= 31) ye = 0xbc, shu = 0x80 >> (AD - 24);
      else if (AD <= 39) ye = 0xbb, shu = 0x80 >> (AD - 32);
      else if (AD <= 47) ye = 0xba, shu = 0x80 >> (AD - 40);
      else if (AD <= 55) ye = 0xb9, shu = 0x80 >> (AD - 48);
      else if (AD <= 63) ye = 0xb8, shu = 0x80 >> (AD - 56);
      if (r == 0)
      {
        d1 = shu;
        d2 = ye;
      }
      if (r != 0)
      {
        d3 = shu;
        d4 = ye;
        if (ye == d2)
        {
          if (shu > d1)
          {
            d5 = shu;
            d5 = d5 >> 1;
            while (d5 != d1)
            { d5 = d5 >> 1;
              shu = shu | (shu >> 1);
            }
          }
          if (shu < d1)
          {
            d5 = shu;
            d5 = d5 << 1;
            while (d5 != d1)
            {
              d5 = d5 << 1;
              shu = shu | (shu << 1);
            }
          }
        }

        if (ye < d2)
        {
          for (q = 0; q < 7; q++)
          {
            shu = shu | (shu << 1);
          }
          shuju();
          ye++;
          while (ye < d2) {
            shu = 0xff, shuju(), ye++;
          }
          if (ye == d2) {
            shu = 0x01;
            if (shu < d1)
            {
              d5 = shu;
              d5 = d5 << 1;
              while (d5 != d1)
              {
                d5 = d5 << 1;
                shu = shu | (shu << 1);
              }
            }
          }
        }
        if (ye > d2)
        {
          for (q = 0; q < 7; q++)
          {
            shu = shu | (shu >> 1);
          }
          shuju();
          ye--;
          while (ye > d2) {
            shu = 0xff, shuju(), ye--;
          }
          if (ye == d2) {
            shu = 0x80;
            if (shu > d1)
            {
              d5 = shu;
              d5 = d5 >> 1;
              while (d5 != d1)
              {
                d5 = d5 >> 1;
                shu = shu | (shu >> 1);
              }
            }
          }
        }
      }
      if (r != 0)
      {
        d1 = d3;
        d2 = d4;
      }
      shuju();
      if (lei != 0x7f) lei++;
    }
  }
}
