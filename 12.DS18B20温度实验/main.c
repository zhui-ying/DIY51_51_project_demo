/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：DS18B20温度实验
* 实验说明 ：数码管显示当前温度
* 实验平台 ：航太51单片机开发板
* 连接方式 ：将DS18B20插到数字温度检测模块的U5上  DS_RD接P22
* 注    意 ：
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint  unsigned int

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
sbit DQ = P2^2;  //定义DS18B20端口DQ  
#define LED_PORT P0
sbit wela_1 = P2^4;
sbit wela_2 = P2^5;
sbit wela_3 = P2^6;
sbit wela_4 = P2^7;

unsigned char temperature=0;
//LED显示字模 0-F 共阳模式
unsigned code table[]= {0Xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
unsigned char RomCode[8];
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
* 函 数 名 ：LEDdisplay
* 函数功能 ：循环显示各个位上的数据
* 输    入 ：num要显示的数据
* 输    出 ：无
*******************************************************************************/
void LEDdisplay(unsigned int num)
{
	unsigned char qian,bai,shi,ge;
	qian=num/1000;
	bai=num%1000/100;
	shi=num%100/10;
	ge=num%10;
	
	wela_1 = 1;	  //关闭所有数码管
	wela_2 = 1;
	wela_3 = 1;
	wela_4 = 1;	

	wela_4=0;  //显示千位
	LED_PORT=table[qian];
	Delayms(1);
	LED_PORT = 0xff;
	wela_4=1;
	
	wela_3=0;  //显示百位
	LED_PORT=table[bai];
	Delayms(1);
	LED_PORT = 0xff;
	wela_3=1;
	
	wela_2=0;  //显示十位
	LED_PORT=table[shi];
	Delayms(1);
	LED_PORT = 0xff;
	wela_2=1;
	
	wela_1=0;  //显示个位
	LED_PORT=table[ge];
	Delayms(1);
	LED_PORT = 0xff;
}

/*******************************************************************/
/*                                                                 */
/*us级延时函数                                                     */
/*                                                                 */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num );
}

/*******************************************************************/
/*                                                                 */
/*初始化ds1820                                                     */
/*                                                                 */
/*******************************************************************/
bit Init_DS18B20(void)
{ 
		bit presence;
     DQ = 1;      //DQ复位
     Delay(8);    //稍做延时

     DQ = 0;      //单片机将DQ拉低
     Delay(90);   //精确延时 大于 480us

     DQ = 1;       //拉高总线
     Delay(8);

     presence = DQ;    //如果=0则初始化成功 =1则初始化失败
     Delay(100);
     DQ = 1; 
     
     return(presence); //返回信号，0=presence,1= no presence
}

/*******************************************************************/
/*                                                                 */
/* 读一个字节                                                      */
/*                                                                 */
/*******************************************************************/
unsigned char ReadOneChar(void)
{
   unsigned char i = 0;
   unsigned char dat = 0;

   for (i = 8; i > 0; i--)
   {
     DQ = 0; // 给脉冲信号
     dat >>= 1;
     DQ = 1; // 给脉冲信号

     if(DQ)
     dat |= 0x80;
     Delay(4);
   }
    return (dat);
}

/*******************************************************************/
/*                                                                 */
/* 写一个字节                                                      */
/*                                                                 */
/*******************************************************************/
void  WriteOneChar(unsigned char dat)
{
  unsigned char i = 0;
  for (i = 8; i > 0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay(5);

    DQ = 1;
    dat>>=1;
  }
}

/*******************************************************************/
/*                                                                 */
/* 温度报警值写入DS18B20                                           */
/*                                                                 */
/*******************************************************************/
void Write_Temperature_alarm(unsigned char Temp_h , unsigned char Temp_l)
{
    Init_DS18B20();
  
    WriteOneChar(0xCC);           //跳过读序号列号的操作 
    WriteOneChar(0x4e);           //将设定的温度报警值写入 DS18B20
	WriteOneChar(Temp_h);  //写TH
	WriteOneChar(Temp_l);  //写TL
    WriteOneChar(0x7f);           //12位精确度
 
	Init_DS18B20();
    WriteOneChar(0xCC);           //跳过读序号列号的操作 
    WriteOneChar(0x48);           //把暂存器里的温度报警值拷贝到EEROM
}

/*******************************************************************/
/*                                                                 */
/* 读取64位序列码                                                  */
/*                                                                 */
/*******************************************************************/
void Read_RomCord(void)
{
   unsigned char j;
   Init_DS18B20();
  
   WriteOneChar(0x33);          // 读序列码的操作
   for (j = 0; j < 8; j++)
   {
     RomCode[j] = ReadOneChar() ; 
   }
}

/*******************************************************************/
/*                                                                 */
/*DS18B20的CRC8校验程序                                            */
/*                                                                 */
/*******************************************************************/
unsigned char CRC8() 
{ 
   unsigned char i,x; 
	unsigned char crcbuff,crc;
   
   crc=0;
   for(x = 0; x <8; x++)
   {
    crcbuff=RomCode[x];
    for(i = 0; i < 8; i++) 
     { 
      if(((crc ^ crcbuff)&0x01)==0) 
      crc >>= 1; 
       else { 
              crc ^= 0x18;   //CRC=X8+X5+X4+1
              crc >>= 1; 
              crc |= 0x80; 
            }         
      crcbuff >>= 1;       
	 }
   }
     return crc;	
}

/*******************************************************************/
/*                                                                 */
/* 读取温度                                                        */
/*                                                                 */
/*******************************************************************/
unsigned char Read_Temperature(void)
{
	unsigned char  i; 
	unsigned char temp_comp;	
	unsigned char temp_data[2];
	unsigned char temp_alarm[2];
	Init_DS18B20();

	WriteOneChar(0xCC);        //跳过读序号列号的操作
	WriteOneChar(0x44);        //启动温度转换

	Init_DS18B20();

	WriteOneChar(0x55);         //匹配ROM命令
	for(i=0;i<8;i++)
	WriteOneChar(RomCode[i]);

	WriteOneChar(0xBE);         //读取温度寄存器

	temp_data[0] = ReadOneChar();   //温度低8位
	temp_data[1] = ReadOneChar();   //温度高8位
	temp_alarm[0] = ReadOneChar();  //温度报警TH
	temp_alarm[1] = ReadOneChar();  //温度报警TL

	temp_comp=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);//取温度整数值
	return temp_comp;  														 
}

void main(void)
{
	int count = 0;
	Read_RomCord();
	while(1)
	{
		if(count >= 100)
		{
			temperature = Read_Temperature();
			count = 0;
		}
		LEDdisplay(temperature);
		count++;
	}
}

