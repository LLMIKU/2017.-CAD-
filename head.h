#ifndef head_h
#define head_h

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>

typedef struct _data
{
	double delta;//电极厚度（mm），占 1 个步长，所有电极厚度相同；
	double n;//电极（包括中间电极与荧光屏，但不包括电位为 0V 的阴极）总数；
	double *zi;//相邻电极之间的距离（mm）数组 ；
	double *Ni;//相邻电极之间要划分的步长数。
	double *di;//电极的长度（mm）；
	double *Li;//电极的要划分的步长数；
	double *Vi;//电极电位
	double r;//电极孔径半径（mm） ，所有电极内孔径半径相同；
	double M;// r 范围内等步长划分的网格数；
	double e;//迭代控制精度；
	double NST;//输出打印空间电位时网格点间隔数，当 NST=1，全部打印；NST=2，隔行隔列打印，依次类推
	double dV;//要求扫描搜索等电位线的电位间隔值，例 V ac =100V 时， V ? =12V，则应扫描电位为 12、24、36、48、60、72、84、96V 的等位线；
}data;
#include "ReadIni.h"
#include "sor.h"

#define filepath D:\\Documents\\Visual Studio 2013\\lq电场等位线\\
//避免了符号单位的影响 2017.10.10_22:38
//避免了单行多个‘=’的影响 2017.10.10_22:38
#endif
