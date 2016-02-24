/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：步进电机试验
* 实验说明 ：单双八拍工作方式：WX1-WX1WX2-WX2-WX2WX3-WX3-WX3WX4-WX4-WX4WX1
* 实验平台 ：航太51单片机开发板 V1.1
* 连接方式 ：CN16接CN7 MOT1接P00  MOT2接P01  MOT3接P02  MOT4接P03
*            步进电机连接方式：蓝线接WX1，粉红线接WX2，黄线接WX3，橙线接WX4,红线接VCC
* 注    意 ：1、本程序用于测试四相五线步进电机常规驱动 
             2、速度不可以调节的过快，不然就没有力矩转动了
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

#define TIME_MS 1 //设定定时时间 ms ,在11.0592M晶振下，不易超过60ms
#define SPEED 2  

//IO接口定义
sbit WX1 = P0^0;	  //定义管脚
sbit WX2 = P0^1;
sbit WX3 = P0^2;
sbit WX4 = P0^3;

//全局变量定义
unsigned char Moter_flag=0;
unsigned char MotorStep=0;
unsigned int  Moter_count=0;

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
* 函 数 名 ：InitMotor
* 函数功能 ：初始化步进电机
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void InitMotor()
{
  WX1 = 1;
  WX2 = 1;
  WX3 = 1;
  WX4 = 1;
}

/*******************************************************************************
* 函 数 名 ：SetMotor
* 函数功能 ：控制步进电机
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void SetMotor()
{
    if(Moter_flag)
	{
		switch(MotorStep)
		{
		   case 0:	
				WX1 = 0;		 // WX1	   
				WX2 = 1;
				WX3 = 1;
				WX4 = 1;
				MotorStep = 1;
		   break;

		   case 1:		 
				WX1 = 0;		 // WX1WX2 
				WX2 = 0;
				WX3 = 1;
				WX4 = 1;
				MotorStep = 2;
		   break;

		   case 2:	   //WX2
				WX1 = 1;
				WX2 = 0;		 
				WX3 = 1;
				WX4 = 1;		   
				MotorStep = 3;
		   break;
		   
		   case 3:		//WX2WX3
				WX1 = 1;
				WX2 = 0;		   
				WX3 = 0;
				WX4 = 1;
				MotorStep = 4;
		   break;
		   case 4:		 //WX3
				WX1 = 1;
				WX2 = 1;		   
				WX3 = 0;
				WX4 = 1;
				MotorStep = 5;
		   break;
		   
		   case 5:			  //WX3WX4
				WX1 = 1;
				WX2 = 1;		    
				WX3 = 0;
				WX4 = 0;
				MotorStep = 6;
		   break;
		   case 6:			  //WX4
				WX1 = 1;
				WX2 = 1;		  
				WX3 = 1;
				WX4 = 0;
				MotorStep = 7;
		   break;
		   case 7:			//WX4WX1
				WX1 = 0;
				WX2 = 1;		   
				WX3 = 1;
				WX4 = 0;
				MotorStep = 0;
		   break;		
		}
		Moter_flag=0;
	}

}



/*******************************************************************************
* 函 数 名 ：Timer0Init
* 函数功能 ：定时器0初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Init()
{
	TMOD=0x01; //设置定时器0工作方式为1
	TH0=(65536-FOSC/12/1000*TIME_MS)/256;
	TL0=(65536-FOSC/12/1000*TIME_MS)%256;
	ET0=1; //开启定时器0中断
	TR0=1;	//开启定时器	
	EA=1;  //打开总中断
}

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	Timer0Init();
	InitMotor();
	while(1)
	{ 
		SetMotor();
	}
}

/*******************************************************************************
* 函 数 名 ：Timer0Int
* 函数功能 ：定时器0中断函数 ， 每隔TIME_MS ms进入
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-FOSC/12/1000*TIME_MS)/256;
	TL0=(65536-FOSC/12/1000*TIME_MS)%256;
	if( Moter_count++ == SPEED) 
	{
	  Moter_flag=1;
	  Moter_count=0;
	}	
}

