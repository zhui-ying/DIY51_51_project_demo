/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：LED流水灯
* 实验说明 ：8个LED灯做循环下移     
* 实验平台 ：航太51单片机开发板
* 连接方式 ：CN5接CN15，按pin编号从小到大连接
* 注    意 ：
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义

//全局变量定义
unsigned char a,b,k;

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
* 函 数 名 ：main
* 函数功能 ：主函数，实现LED灯从上到下循环移动
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	k=0xfe;
	P1=k;
	while(1)
	{
		Delayms(1000);
		k=_crol_(k,1);//循环左移，右边补1
		P1=k;
	}
}
