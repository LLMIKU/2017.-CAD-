#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>

#define filePath "D:\\CADtest\\1120140882\\"
#define fileName "1120140882_刘琦.dat"
#define outFileName "1120140882_刘琦.res"

typedef struct _data
{
	double delta;//电极厚度（mm），占 1 个步长，所有电极厚度相同；
	double n;//电极（包括中间电极与荧光屏，但不包括电位为 0V 的阴极）总数；
	double *zi;//相邻电极之间的距离（mm）数组 ；
	double *Ni;//相邻电极之间要划分的步长数。
	double *di;//电极的长度（mm）；
	double *Li;//电极的要划分的步长数；
	double *Vi;//电极电位
	double r;//电极孔径半径（mm），所有电极内孔径半径相同；
	double M;// r 范围内等步长划分的网格数；
	double e;//迭代控制精度；
	double NST;//输出打印空间电位时网格点间隔数，当 NST=1，全部打印；NST=2，隔行隔列打印，依次类推
	double *dV;//要求扫描搜索等电位线的电位间隔值，例 V ac =100V 时， V ? =12V，则应扫描电位为 12、24、36、48、60、72、84、96V 的等位线；
}data;

/*----------文件读入模块----------*/
void charToNum(char *inCh, double *outNum)//字符转数字
{
	double num = 0;
	switch (*inCh)
	{
	case '0':num = 0; break;
	case '1':num = 1; break;
	case '2':num = 2; break;
	case '3':num = 3; break;
	case '4':num = 4; break;
	case '5':num = 5; break;
	case '6':num = 6; break;
	case '7':num = 7; break;
	case '8':num = 8; break;
	case '9':num = 9; break;
	default:num = 0; break;
	}
	*outNum = num;
}

void strToNum(char *inStr, int strLength, int intCount, double *outNum)//字符串转数字
{
	int i = 0, j = 0;
	int k = 0;
	double num = 0;
	char *tmp = inStr;
	for (i = 0; i < strLength; i++, tmp++)
	{
		charToNum(tmp, &num);
		for (j = k; j < intCount - 1; j++)//j=k
		{
			if (*tmp >= '0'&&*tmp <= '9')//避免符号单位等影响
			{
				num = num * 10;
				//k++;
			}
		}
		if (*tmp >= '0'&&*tmp <= '9')//避免符号单位等影响
		{
			k++;
		}
		(*outNum) += num;
	}
}

int TransNumbers(char *StrTmp, double *Number)//把传入字符串转换为数字
{
	int ret = 0;
	int intCount = 0;//整数位数
	int intCount2 = 0;//小数点后0之后的整数位数
	int length = 0;//字符长度
	int i = 0, j = 0;
	int deciFlag = 0;//小数点标志
	int deciCount = 0;//小数点后0的个数
	double deciNum = 0;//小数部分
	double num = 0;
	char *tmp = StrTmp;
	//读取整数部分
	while (*tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')//避免符号单位等影响数字位数计数
			intCount++;
		if (*tmp == '.')
			break;
		length++;
		tmp++;
	}
	tmp = StrTmp;
	strToNum(tmp, length, intCount, Number);
	//读取小数部分
	tmp = strstr(StrTmp, ".");
	if (tmp == NULL)
		return ret;
	deciCount = 1;//小数点计入
	while (*++tmp)
	{
		if (*tmp != '0')
			break;
		deciCount++;//计算小数点及之后连续0的位数
	}
	tmp = strstr(StrTmp, ".");
	tmp += deciCount;//跳到小数点后连续0后的整数部分
	length = 0;
	while (*tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')//避免符号单位等影响数字位数计数
			intCount2++;
		length++;
		tmp++;
	}
	tmp = strstr(StrTmp, ".");
	tmp += deciCount;
	strToNum(tmp, length, intCount2, &deciNum);
	for (i = 0; i < deciCount + intCount2 - 1; i++)
	{
		deciNum = deciNum / 10;
	}
	//整数部分与小数部分相加
	(*Number) += deciNum;
	return ret;
}

//读取文件里的具体参数
int ReadIni(char *filename, char *part, 
	double *partValue, data **datSec, int *arrayLen)
{
	int ret = 0;
	int i = 0;
	*arrayLen = 1;
	char str1[200] = { 0 };
	char str2[220] = { 0 };
	strcpy(str2, filePath);
	strcpy(str1, filename);
	//sprintf(str2, "D:\\CADtest\\1120140882\\%s", str1);
	strcat(str2, str1);
	FILE *fp = fopen(str2, "r+");
	if (fp == NULL)
	{
		printf("fun ReadIni() err:can not open the file\n");
		ret = -1;
		return ret;
	}
	char str3[1024] = { 0 };
	int flag = 0;
	char filestr[1024] = { 0 };
	char partValueTmp[128] = { 0 };
	while (!feof(fp))
	{
		char *p = NULL;
		char *ptmp = NULL, *pBegain = NULL, *pEnd = NULL;
		fgets(str3, 256, fp);
		strcat(filestr, str3);
		int n = strlen(str3);
		str3[n + 1] = ';';//手动在字符串'\0'后添加';'结束符，防止字符串'='后什么都没有而中断出错
		p = strstr(str3, part);
		if (p != NULL)
		{
			flag = 1;//找到了配置项
			ptmp = strchr(str3 + (p - str3), '=');//在配置项后找‘=’号
			if (ptmp == NULL)
			{
				system("color D");
				printf("配置项%s没有'='！\n", part);
				system("pause");
				ret = -2;
				goto End;
			}
			ptmp = ptmp + 1;
			while (1)//读取配置项
			{
				if (*ptmp == ' ')
				{
					ptmp++;
				}
				else
				{
					if (*ptmp == '\n' || *ptmp == ';' || *ptmp == '；' || *ptmp == '\0')
					{
						system("color B");
						printf("配置项%s没有配置初始值！\n", part);
						system("pause");
						ret = -3;
						goto End;
					}
					pBegain = ptmp;
					break;
				}
			}
			while (1)
			{
				if (*ptmp == ';' || *ptmp == '；' || *ptmp == '\n'
					|| *ptmp == '\0')//删除了 || *ptmp == ' '
				{
					break;
				}
				else
					ptmp++;
			}
			pEnd = ptmp;
			memcpy(partValueTmp, pBegain, pEnd - pBegain);
			int dotCount = 0;
			partValueTmp[pEnd - pBegain] = '\0';
			ptmp = partValueTmp;
			while (*ptmp)
			{
				if (*ptmp == ',' || *ptmp == '，')
					dotCount++;
				ptmp++;
			}
			if (partValue == NULL)
			{
				char *pEnd2 = NULL, *pBegain2 = NULL;
				partValue = (double *)malloc((dotCount + 1)*sizeof(double));
				*arrayLen = dotCount + 1;
				memset(partValue, 0, (dotCount + 1)*sizeof(double));
				switch (*part)
				{
				case 'z':(*datSec)->zi = partValue; break;
				case 'N':(*datSec)->Ni = partValue; break;
				case 'd':(*datSec)->di = partValue; break;
				case 'L':(*datSec)->Li = partValue; break;
				case 'V':(*datSec)->Vi = partValue; break;
				case 'b':(*datSec)->dV = partValue; break;
				default:printf("partValue not correspond ii\n"); system("pause");
					break;
				}
				pBegain2 = partValueTmp;
				for (i = 0; i < dotCount + 1; i++)
				{
					ptmp = pBegain2;
					while (1)
					{
						if (*ptmp == ',' || *ptmp == ';' || *ptmp == '；'
							|| *ptmp == '\0' || *ptmp == '\n')
							break;
						ptmp++;
					}
					pEnd2 = ptmp;
					memcpy(partValueTmp, pBegain2, pEnd2 - pBegain2);
					partValueTmp[pEnd2 - pBegain2] = '\0';
					ret = TransNumbers(partValueTmp, partValue + i);
					pBegain2 = pEnd2 + 1;
				}
				break;
			}
			else if (*partValue == 0)//如果传入的partValue地址的内容是0，说明传入的是数据
			{
				ret = TransNumbers(partValueTmp, partValue);
				if (ret != 0)
				{
					printf("fun ReadIni() err: TransNumbers()\n");
				}
				break;
			}
		}
	}
End:
	if (flag == 0)
	{
		system("color D");
		printf("没有找到%s配置项\n", part);
		system("pause");
		ret = -4;
	}
	fclose(fp);
	return ret;
}

/*----------SOR法模块----------*/
int sor(int M, int N, data *dat, double **E, double *can,
	double *InW, int *InCanCount, int *pre, int *next)
{
	int ret = 0;
	int i = 0, j = 0, k = 0;
	double h1 = 0, h2 = 0, h3 = 0, h4 = 0;
	double c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0;
	double r0 = 0;
	double w = *InW;
	double tmp = 0;
	int foreverFlag = 0;

	double canAvg = 0;
	double canAll = 0;
	int canCount = 0;
	for (i = M - 1; i > 0; i--)
	{
		if (i == 1)
		{
			h3 = dat->r / dat->M;
			h4 = dat->delta;
			r0 = (M - 1 - i)*dat->r / dat->M;
		}
		else if (i == M - 1)
		{
			h3 = 0;
			h4 = dat->r / dat->M;
			r0 = 0;
		}
		else
		{
			h3 = dat->r / dat->M;
			h4 = dat->r / dat->M;
			r0 = (M - 1 - i)*dat->r / dat->M;
		}

		for (j = 1; j < N - 1; j++)
		{
			foreverFlag = 0;
			for (k = 0; k < (int)dat->n - 1; k++)
			{
				if (j < pre[0])//第一个电极左边但不包括最左边
				{
					h1 = (dat->zi)[0] / (dat->Ni)[0];
					h2 = h1;
					break;
				}
				else if (j == pre[k] && i != 1)//电极边界
				{
					h1 = (dat->zi)[k] / (dat->Ni)[k];
					h2 = (dat->di)[k] / (dat->Li)[k];
					break;
				}
				else if (j == next[k] && i != 1)//电极边界
				{
					h1 = (dat->di)[k] / (dat->Li)[k];
					h2 = (dat->zi)[k + 1] / (dat->Ni)[k + 1];
					break;
				}
				else if (j > pre[k] && j < next[k] && i != 1)//电极区域内
				{
					h1 = (dat->di)[k] / (dat->Li)[k];
					h2 = h1;
					break;
				}
				else if (j > next[(int)dat->n - 2])//最后电极的左边且不包括最后电极
				{
					h1 = (dat->zi)[(int)dat->n - 1] / (dat->Ni)[(int)dat->n - 1];
					h2 = (dat->zi)[(int)dat->n - 1] / (dat->Ni)[(int)dat->n - 1];
					break;
				}
				else if (j > next[k] && j < pre[k + 1])//电极之间
				{
					h1 = (dat->zi)[k + 1] / (dat->Ni)[k + 1];
					h2 = (dat->zi)[k + 1] / (dat->Ni)[k + 1];
					break;
				}
				else
				{
					if (i == 1)
						foreverFlag = 1;
					break;
				}
			}
			if (foreverFlag == 1)
				continue;
			c1 = 2 / (h1 * (h1 + h2));
			c2 = 2 / (h2 * (h1 + h2));
			if (i == M - 1)//如果是轴上点
			{
				c3 = 0;
				c4 = 4 / (h4*h4);
			}
			else
			{
				c3 = (2 * r0 - h4) / (r0*h3*(h3 + h4));
				c4 = (2 * r0 + h3) / (r0*h4*(h3 + h4));
			}
			c0 = c1 + c2 + c3 + c4;
			tmp = E[i][j];
			if (i == M - 1)//轴上点
			{
				E[i][j] = (1 - w)*E[i][j] +
					w*(c1*E[i][j - 1] + c2*E[i][j + 1] + c4*E[i - 1][j]) / c0;
			}
			else
			{
				E[i][j] = (1 - w)*E[i][j] +
					w*(c1*E[i][j - 1] + c2*E[i][j + 1] +
					c3*E[i + 1][j] + c4*E[i - 1][j]) / c0;
			}
			can[canCount] = E[i][j] - tmp;
			if (can[canCount] < 0)
				can[canCount] = 0 - can[canCount];
			canCount++;
		}
	}
	*InCanCount = canCount;
	*InW = w;
	return ret;
}

/*----------主体程序----------*/
void InitialData(data *dat)//结构体初始化
{
	dat->delta = 0;
	dat->n = 0;
	dat->zi = NULL;
	dat->Ni = NULL;
	dat->di = NULL;
	dat->Li = NULL;
	dat->Vi = NULL;
	dat->r = 0;
	dat->M = 0;
	dat->e = 0;
	dat->NST = 0;
	dat->dV = NULL;
}

void main()
{
	double **E = NULL;
	int i, j, k;
	int M = 0, N = 0;
	FILE *fpOut = NULL;
	char outFile[256] = filePath;
	strcat(outFile, outFileName);
	int arrayLen = 0;
	int datN = 0;
	data *dat = (data *)malloc(sizeof(data));
	fpOut = fopen(outFile, "w");
	InitialData(dat);

	fprintf(fpOut, "这是输出文件\n");
	ReadIni(fileName, "δ", &dat->delta, &dat, &arrayLen);
	fprintf(fpOut, "δ=%f\n", dat->delta);

	ReadIni(fileName, "n", &dat->n, &dat, &arrayLen);
	fprintf(fpOut, "n=%f\n", dat->n);
	datN = (int)dat->n;

	ReadIni(fileName, "zi", dat->zi, &dat, &arrayLen);
	if (datN != (int)arrayLen)
	{
		system("color B");
		printf("电极之间的距离zi的个数与电极数n不匹配,zi个数应和n个数相同\n");
		system("pause");
		return;
	}
	for (i = 0; i < arrayLen; i++)
	{
		fprintf(fpOut, "z%d=%f ", i + 1, (dat->zi)[i]);
	}
	fprintf(fpOut, "\n");

	ReadIni(fileName, "Ni", dat->Ni, &dat, &arrayLen);
	if (datN != (int)arrayLen)
	{
		system("color B");
		printf("电极之间的步长数Ni的个数与电极数n不匹配,Ni个数应和n个数相同\n");
		system("pause");
		return;
	}
	for (i = 0; i < arrayLen; i++)
	{
		fprintf(fpOut, "N%d=%f ", i + 1, (dat->Ni)[i]);
		N += (int)(dat->Ni)[i];
	}
	fprintf(fpOut, "\n");

	ReadIni(fileName, "di", dat->di, &dat, &arrayLen);
	if (datN != (int)(arrayLen + 1))
	{
		system("color B");
		printf("电极长度di的个数与电极数n不匹配，di个数应比电极个数少一个\n");
		system("pause");
		return;
	}
	for (i = 0; i < arrayLen; i++)
	{
		fprintf(fpOut, "d%d=%f ", i + 1, (dat->di)[i]);
	}
	fprintf(fpOut, "\n");

	ReadIni(fileName, "Li", dat->Li, &dat, &arrayLen);
	if (datN != (int)(arrayLen + 1))
	{
		system("color B");
		printf("电极划分步长数Li的个数与电极数n不匹配,Li个数应比电极个数少一个\n");
		system("pause");
		return;
	}
	for (i = 0; i < arrayLen; i++)
	{
		fprintf(fpOut, "L%d=%f ", i + 1, (dat->Li)[i]);
		N += (int)(dat->Li)[i];
	}
	fprintf(fpOut, "\n");

	ReadIni(fileName, "Vi", dat->Vi, &dat, &arrayLen);
	if (datN != (int)(arrayLen))
	{
		system("color B");
		printf("电极电位Vi的个数与电极数n不匹配\n");
		system("pause");
		return;
	}
	for (i = 0; i < arrayLen; i++)
	{
		fprintf(fpOut, "V%d=%f ", i + 1, (dat->Vi)[i]);
	}
	fprintf(fpOut, "\n");

	ReadIni(fileName, "r", &dat->r, &dat, &arrayLen);
	fprintf(fpOut,"r=%f\n", dat->r);

	ReadIni(fileName, "M", &dat->M, &dat, &arrayLen);
	fprintf(fpOut, "M=%f\n", dat->M);
	M = (int)dat->M + 2;

	ReadIni(fileName, "ε", &dat->e, &dat, &arrayLen);
	fprintf(fpOut, "ε=%f\n", dat->e);

	ReadIni(fileName, "NST", &dat->NST, &dat, &arrayLen);
	fprintf(fpOut, "NST=%f\n", dat->NST);

	ReadIni(fileName, "b", dat->dV, &dat, &arrayLen);

	int bLen = arrayLen;
	if (bLen == 1)
		fprintf(fpOut, "扫描电位间隔为：%fV", dat->dV[0]);
	else for (i = 0; i < bLen; i++)
		fprintf(fpOut, "scanV%d=%f ", i + 1, dat->dV[i]);
	fprintf(fpOut, "\n\n");
	N = N + 1;

	//电场初始化
	E = (double **)malloc(M*sizeof(double *));
	for (i = 0; i < M; i++)
	{
		E[i] = (double *)malloc(N*sizeof(double));
	}
	int *pre = (int *)malloc(((int)dat->n - 1)*sizeof(int));
	memset(pre, 0, ((int)dat->n - 1)*sizeof(int));
	int *next = (int *)malloc(((int)dat->n - 1)*sizeof(int));
	memset(next, 0, ((int)dat->n - 1)*sizeof(int));
	for (k = 0; k < (int)dat->n - 1; k++)
	{
		next[k] = (int)((dat->Ni)[k] + (dat->Li)[k]);
		if (k>0) next[k] += next[k - 1];
		pre[k] = (int)(next[k] - (dat->Li)[k]);
	}
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (j == 0)//最左边的点即阴极的电位
				E[i][j] = 0;
			else if (j == N - 1)//最右边，最后一个电极即荧光屏上的电位
				E[i][j] = dat->Vi[(int)dat->n - 1];
			else
				for (k = 0; k < (int)dat->n - 1; k++)
				{
				if (j >= pre[k] && j <= next[k])//电极内
				{
					E[i][j] = (dat->Vi)[k];
					break;
				}
				else if (j < pre[0])//第一个电极左边且不包括最左边
				{
					E[i][j] = (dat->Vi)[0] * (double)j / dat->Ni[0];
					break;
				}
				else if (j > next[(int)dat->n - 2])//最后电极的左边且不包括最后电极
				{
					E[i][j] = ((dat->Vi)[(int)dat->n - 1] - (dat->Vi)[(int)dat->n - 2]) *
						(double)(j - next[(int)(dat->n) - 2])
						/ dat->Ni[(int)(dat->n) - 1] + (dat->Vi)[(int)dat->n - 2];
					break;
				}
				else if (j >= next[k] && j <= pre[k + 1])
				{
					E[i][j] = ((dat->Vi)[k + 1] - (dat->Vi)[k]) *
						(double)(j - next[k])
						/ dat->Ni[k + 1] + (dat->Vi)[k];
					break;
				}
				}

		}
	}
	/*for (i = 0; i < M; i++)//初始化后各点电位输出测试
	{
	for (j = 0; j < N; j++)
	{
	printf("%.2f ", E[i][j]);
	}
	printf("\n");
	}
	printf("\n");
	system("pause");*/

	//固定w
	double *can = (double *)malloc(M*N*sizeof(double));
	memset(can, 0, M*N*sizeof(double));
	int canCount = 0;
	double canAll = 0;
	double canAllTmp = 0;
	double w = 1;
	double lambda = 0;
	double u = 0;
	double wTmp = 0;
	double limit = 0;
	int eFlag = 1;
	double calCan(int canCount, double *can);
	int time = 0;
	sor(M, N, dat, E, can, &w, &canCount, pre, next);
	w = 1.375;
	while (1)
	{
		for (i = 0; i < 12; i++)
		{
			sor(M, N, dat, E, can, &w, &canCount, pre, next);
			if (i == 10)
				canAllTmp = calCan(canCount, can);
			if (i == 11)
				canAll = calCan(canCount, can);
			canCount = 0;
		}
		lambda = canAll / canAllTmp;
		u = (lambda + w - 1) / (sqrt(lambda)*w);
		wTmp = w;
		fprintf(fpOut, "lambda:%f  u:%f  w:%f\n", lambda, u, wTmp);
		w = 2 / (1 + sqrt(1 - u*u));
		w = 1.25*w - 0.5;
		canAll = 0;
		canAllTmp = 0;
		limit = fabs((w + wTmp) / 2 - w) / fabs(2 - w);
		time++;
		if (limit <= 0.05)
			break;
	}
	fprintf(fpOut, "w is %f\n", w);
	fprintf(fpOut, "求解w迭代次数为12次的 %d 倍\n", time);
	time = 0;

	//迭代求解电位
	while (eFlag)
	{
		eFlag = 0;
		time++;
		sor(M, N, dat, E, can, &w, &canCount, pre, next);
		for (i = 0; i < canCount - 1; i++)
		{
			if (can[i]>=dat->e)
			{
				eFlag = 1;
				break;
			}
		}
	}
	canAll=calCan(canCount, can);
	double canAvg = canAll / (double)canCount;
	double maxCan = 0;
	for (i = 0; i < canCount - 1; i++)
	{
		if (maxCan < can[i])
			maxCan = can[i];
	}
	fprintf(fpOut, "迭代求解成功！\n");
	fprintf(fpOut, "迭代次数为 %d 次\n", time);
	fprintf(fpOut, "迭代退出时平均残差为%f，最大残差为%f\n\n", canAvg, maxCan);
	fprintf(fpOut, "以下是迭代求解出的网格点上的电位值:\n");
	fprintf(fpOut, "其中第0行是电极所在的边界:\n");
	for (i = 0; i < M;)//迭代最终各点电位输出测试
	{
		fprintf(fpOut, "第%d行：", i);
		for (j = 0; j < N;)
		{
			fprintf(fpOut, "(%d,%d)%f  ", i, j, E[i][j]);
			if (dat->NST == 2.0)
				j += 2;
			else
				j++;
		}
		fprintf(fpOut, "\n");
		if (dat->NST == 2.0)
			i += 2;
		else
			i++;
	}

	k = 0;
	int multi = 0;
	int multiM = 0;
	int multiN = 0;
	double b = 0;
	double t = 50;
	for (i = 1; i < 50; i++)
	{
		multi = i;
		multiM = M*i;
		multiN = N*i;
		if (multiM>1000 || multiN > 1000)
			break;
	}
	initgraph(multiN, multiM);
	int printLine(int M, int N, double b, double **E, int multi);
	i = 0;
	if (bLen == 1)//如果只有一个要搜索的电位值
		for (b = dat->dV[0]; b < 100; b = b + dat->dV[0])
		{
		setcolor(RGB(255 - 2 * b, b, 2 * b));
		printLine(M, N, b, E, multi);
		Sleep(1);
		}
	else for (i = 0; i < bLen; i++)//有多个电位搜索值
	{
		b = dat->dV[i];
		setcolor(RGB(255 - 2 * b, b, 2 * b));
		printLine(M, N, dat->dV[i], E, multi);
		Sleep(1);
	}
	for (i = 0; i < M; i++)
	{
		free(E[i]);
	}
	free(E);
	free(pre);
	free(next);
	free(can);
	_getch();
	exit(0);
}

//计算残差和
double calCan(int canCount, double *can)
{
	double canAll = 0;
	int i;
	for (i = 0; i < canCount - 1; i++)
	{
		canAll += can[i];
	}
	return canAll;
}

/*----------打印电位值为b的等位线模块----------*/
int printLine(int M, int N, double b, double **E, int multi)
{
	int i, j;
	int flag = 0;
	int maxFlagX = 0;
	int maxFlagY = 0;
	int iFlag = 0;
	for (i = 0; i < M; i++)
	{
		flag = 0;
		iFlag = 0;
		for (j = 0; j < N - 1; j++)
		{
			if ((E[i][j] < b && E[i][j + 1] > b) || (E[i][j] > b && E[i][j + 1] < b) || E[i][j] == b)
			{
				flag++;
				if (maxFlagX < flag)
					maxFlagX = flag;
				iFlag = 1;
			}
		}
		/*if (iFlag == 0)
		{
			printf("第%d行没有找到该电位 %.2f V\n", i, b);
			system("pause");
		}*/
	}
	for (j = 0; j < N; j++)
	{
		flag = 0;
		for (i = 0; i < M - 1; i++)
		{
			if ((E[i][j] < b && E[i + 1][j] > b) || (E[i][j] > b && E[i + 1][j] < b)
				|| E[i][j] == b)
			{
				flag++;
				if (maxFlagY < flag)
					maxFlagY = flag;
			}
		}
	}

	int **lineX = (int **)malloc((maxFlagX + maxFlagY)*sizeof(int *));
	int **lineY = (int **)malloc((maxFlagX + maxFlagY)*sizeof(int *));
	for (i = 0; i < maxFlagX + maxFlagY; i++)
	{
		lineX[i] = (int *)malloc((M + N)*sizeof(int));
		memset(lineX[i], 0, (M + N)*sizeof(int));
	}
	for (i = 0; i < maxFlagX + maxFlagY; i++)
	{
		lineY[i] = (int *)malloc((M + N)*sizeof(int));
		memset(lineY[i], 0, (M + N)*sizeof(int));
	}
	int *k = (int *)malloc((maxFlagX + maxFlagY)*sizeof(int));
	memset(k, 0, (maxFlagX + maxFlagY)*sizeof(int));
	for (i = 0; i < M; i++)
	{
		flag = 0;
		for (j = 0; j < N - 1; j++)//行扫描
		{
			if (E[i][j] == b)
			{
				lineX[flag][k[flag]] = j * multi;
				lineY[flag][k[flag]] = i * multi;
				k[flag]++;
				flag++;
				//continue;
			}
			else if ((E[i][j] < b && E[i][j + 1] > b) || (E[i][j] > b && E[i][j + 1] < b))
			{
				lineX[flag][k[flag]] = j * multi +
					(int)((b - E[i][j]) / (E[i][j + 1] - E[i][j]) * multi);
				lineY[flag][k[flag]] = i * multi;
				k[flag]++;
				flag++;
			}
		}
	}
	int flagTmp = maxFlagX;
	int check(int i, int j, int multi, int Inflag, int **lineX, int **lineY, int *k);
	int checki = 0, checkj = 0;
	for (j = 0; j < N; j++)
	{
		flag = flagTmp;
		for (i = 0; i < M - 1; i++)
		{
			if (E[i][j] == b)
			{
				checki = i*multi;
				checkj = j*multi;
				if (check(checki, checkj, multi, maxFlagX, lineX, lineY, k))
				{
					lineX[flag][k[flag]] = j * multi;
					lineY[flag][k[flag]] = i * multi;
					k[flag]++;
					flag++;
				}
			}
			else if ((E[i][j] < b && E[i + 1][j] > b) || (E[i][j] > b && E[i + 1][j] < b))
			{
				checki = i * multi +
					(int)((b - E[i][j]) / (E[i + 1][j] - E[i][j]) * multi);
				checkj = j * multi;
				if (check(checki, checkj, multi, maxFlagX, lineX, lineY, k))
				{
					lineX[flag][k[flag]] = checkj;
					lineY[flag][k[flag]] = checki;
					k[flag]++;
					flag++;
				}
			}
		}
	}
	int minD = 2 * multi*multi;
	int minX = 0;
	int minY = 0;
	int distance = 0;
	int m = 0;
	int minZu = 0;

	for (flag = 0; flag < maxFlagX; flag++)//横向自连
	{
		for (i = 0; i < k[flag]; i++)
		{
			minD = M*M*multi*multi + N*N*multi*multi;
			minX = M*multi;
			minY = M*multi;
			for (j = flag; j < maxFlagX; j++)//从flag当前组开始找，否则前后找的一致会导致断点
			{
				for (m = 0; m < k[j]; m++)
				{

					if (j == flag&&m == i)
						break;
					distance = (lineX[flag][i] - lineX[j][m])*(lineX[flag][i] - lineX[j][m]) +
						(lineY[flag][i] - lineY[j][m])*(lineY[flag][i] - lineY[j][m]);
					if (distance < minD)
					{
						minD = distance;
						minX = lineX[j][m];
						minY = lineY[j][m];
						minZu = j;
					}
				}
			}
			if (minD < 3 * multi*multi)
				line(lineX[flag][i], lineY[flag][i], minX, minY);
		}
	}

	for (flag = maxFlagX; flag < maxFlagX + maxFlagY; flag++)//纵向自连
	{
		for (i = 0; i < k[flag]; i++)//第一个点和最后一个点不连
		{
			minD = M*M*multi*multi + N*N*multi*multi;
			minX = 0;
			minY = 0;
			for (j = flag; j < maxFlagX + maxFlagY; j++)//j=maxFlagX
			{
				for (m = 0; m < k[j]; m++)
				{
					if (j == flag&&m == i)
						break;//break表示和之前的点相连
					distance = (lineX[flag][i] - lineX[j][m])*(lineX[flag][i] - lineX[j][m]) +
						(lineY[flag][i] - lineY[j][m])*(lineY[flag][i] - lineY[j][m]);
					if (distance < minD)
					{
						minD = distance;
						minX = lineX[j][m];
						minY = lineY[j][m];
					}
				}
			}
			if (minD < 3 * multi*multi)//当距离不是太大的时候才能连
				line(lineX[flag][i], lineY[flag][i], minX, minY);
		}
	}

	for (flag = maxFlagX; flag < maxFlagX + maxFlagY; flag++)//纵向与横向相连
	{
		for (i = 0; i < k[flag]; i++)//第一个点和最后一个点不连
		{
			minD = M*M*multi*multi + N*N*multi*multi;
			minX = 0;
			minY = 0;
			for (j = 0; j < maxFlagX; j++)//j=maxFlagX
			{
				for (m = 0; m < k[j]; m++)
				{
					distance = (lineX[flag][i] - lineX[j][m])*(lineX[flag][i] - lineX[j][m]) +
						(lineY[flag][i] - lineY[j][m])*(lineY[flag][i] - lineY[j][m]);
					if (distance < minD)
					{
						minD = distance;
						minX = lineX[j][m];
						minY = lineY[j][m];
					}
				}
			}
			if (minD < 3 * multi*multi)//当距离不是太大的时候才能连
				line(lineX[flag][i], lineY[flag][i], minX, minY);
		}
	}

	return 0;
}

int check(int i, int j, int multi, int Inflag, int **lineX, int **lineY, int *k)
{
	/*int flag = 0;
	int m;*/
	//for (flag = 0; flag < Inflag; flag++)
	//{
	//	for (m = 0; m < k[flag]; m++)
	//	{
	//		if ((lineX[flag][m] == j&&
	//			lineY[flag][m] == i) /*||
	//			((lineX[flag][m] - j)*(lineX[flag][m] - j) +
	//			(lineY[flag][m] - i)*(lineY[flag][m] - i)<1* multi*multi)*/
	//			)
	//			return 0;
	//	}
	//}
	return 1;
}
