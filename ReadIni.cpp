#include "head.h"

void charToNum(char *inCh, double *outNum)
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

void strToNum(char *inStr, int strLength, int intCount, double *outNum)
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

int TransNumbers(char *StrTmp, double *Number)
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

int ReadIni(char *filename, char *part, double *partValue, data **datSec, int *arrayLen)
{
	int ret = 0;
	int i = 0;
	*arrayLen = 1;
	char str1[200] = { 0 };
	char str2[220] = { 0 };
	strcpy(str1, filename);
	sprintf(str2, "D:\\Documents\\Visual Studio 2013\\lq电场等位线\\%s", str1);
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