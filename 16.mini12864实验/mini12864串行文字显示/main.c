/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：12864串行文字显示实验
* 实验说明 ：字符串从上到下刷新显示
* 实验平台 ：航太51单片机开发板
* 连接方式 ：mini12864插到LCD1602接口上
* 注    意 ：mini12864朝里放，也就是mini12864 GND引脚对应接口1，mini12864和LCD1602接口在硬件上是兼容的
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include<reg51.h>
#include"uc1705.h"

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
sbit DULA = P2^6;
sbit WELA = P2^7;
void Delay10ms(unsigned int c);

/*******************************************************************************
* 函 数 名 ：Delayms
* 函数功能 ：实现 ms级的延时
* 输    入 ：ms
* 输    出 ：无
*******************************************************************************/
void Delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
	#if FOSC == 11059200L
		for(j=0;j<114;j++);
	#elif FOSC == 12000000L
	  for(j=0;j<123;j++);
	#elif FOSC == 24000000L
		for(j=0;j<249;j++);
	#else
		for(j=0;j<114;j++);
	#endif
}

/*******************************************************************************
* 函 数 名: main
* 函数功能: 主函数
* 输    入: 无
* 输    出: 无
*******************************************************************************/
void main()
{
	uchar i = 128;
	Lcd12864_Init();
	LcdClearScreen();
	DULA = 1;
	P0 = 0xff;
	DULA = 0;
	WELA = 1;
	P0 = 0xff;
	WELA = 0;
		
	while (1)
	{
		for (i=0; i<8; i += 2)
		{
			LcdClearScreen();
			LcdWriteString(0, i, "航太电子有限公司");
			Delayms(1000);
		}
	
	}
}

