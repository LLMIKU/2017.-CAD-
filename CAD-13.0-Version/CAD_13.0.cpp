#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>

#define filePath "D:\\CADtest\\1120140882\\"
#define fileName "1120140882_����.dat"
#define outFileName "1120140882_����.res"

typedef struct _data
{
	double delta;//�缫��ȣ�mm����ռ 1 �����������е缫�����ͬ��
	double n;//�缫�������м�缫��ӫ����������������λΪ 0V ��������������
	double *zi;//���ڵ缫֮��ľ��루mm������ ��
	double *Ni;//���ڵ缫֮��Ҫ���ֵĲ�������
	double *di;//�缫�ĳ��ȣ�mm����
	double *Li;//�缫��Ҫ���ֵĲ�������
	double *Vi;//�缫��λ
	double r;//�缫�׾��뾶��mm�������е缫�ڿ׾��뾶��ͬ��
	double M;// r ��Χ�ڵȲ������ֵ���������
	double e;//�������ƾ��ȣ�
	double NST;//�����ӡ�ռ��λʱ������������� NST=1��ȫ����ӡ��NST=2�����и��д�ӡ����������
	double *dV;//Ҫ��ɨ�������ȵ�λ�ߵĵ�λ���ֵ���� V ac =100V ʱ�� V ? =12V����Ӧɨ���λΪ 12��24��36��48��60��72��84��96V �ĵ�λ�ߣ�
}data;

/*----------�ļ�����ģ��----------*/
void charToNum(char *inCh, double *outNum)//�ַ�ת����
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

void strToNum(char *inStr, int strLength, int intCount, double *outNum)//�ַ���ת����
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
			if (*tmp >= '0'&&*tmp <= '9')//������ŵ�λ��Ӱ��
			{
				num = num * 10;
				//k++;
			}
		}
		if (*tmp >= '0'&&*tmp <= '9')//������ŵ�λ��Ӱ��
		{
			k++;
		}
		(*outNum) += num;
	}
}

int TransNumbers(char *StrTmp, double *Number)//�Ѵ����ַ���ת��Ϊ����
{
	int ret = 0;
	int intCount = 0;//����λ��
	int intCount2 = 0;//С�����0֮�������λ��
	int length = 0;//�ַ�����
	int i = 0, j = 0;
	int deciFlag = 0;//С�����־
	int deciCount = 0;//С�����0�ĸ���
	double deciNum = 0;//С������
	double num = 0;
	char *tmp = StrTmp;
	//��ȡ��������
	while (*tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')//������ŵ�λ��Ӱ������λ������
			intCount++;
		if (*tmp == '.')
			break;
		length++;
		tmp++;
	}
	tmp = StrTmp;
	strToNum(tmp, length, intCount, Number);
	//��ȡС������
	tmp = strstr(StrTmp, ".");
	if (tmp == NULL)
		return ret;
	deciCount = 1;//С�������
	while (*++tmp)
	{
		if (*tmp != '0')
			break;
		deciCount++;//����С���㼰֮������0��λ��
	}
	tmp = strstr(StrTmp, ".");
	tmp += deciCount;//����С���������0�����������
	length = 0;
	while (*tmp)
	{
		if (*tmp >= '0'&&*tmp <= '9')//������ŵ�λ��Ӱ������λ������
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
	//����������С���������
	(*Number) += deciNum;
	return ret;
}

//��ȡ�ļ���ľ������
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
		str3[n + 1] = ';';//�ֶ����ַ���'\0'�����';'����������ֹ�ַ���'='��ʲô��û�ж��жϳ���
		p = strstr(str3, part);
		if (p != NULL)
		{
			flag = 1;//�ҵ���������
			ptmp = strchr(str3 + (p - str3), '=');//����������ҡ�=����
			if (ptmp == NULL)
			{
				system("color D");
				printf("������%sû��'='��\n", part);
				system("pause");
				ret = -2;
				goto End;
			}
			ptmp = ptmp + 1;
			while (1)//��ȡ������
			{
				if (*ptmp == ' ')
				{
					ptmp++;
				}
				else
				{
					if (*ptmp == '\n' || *ptmp == ';' || *ptmp == '��' || *ptmp == '\0')
					{
						system("color B");
						printf("������%sû�����ó�ʼֵ��\n", part);
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
				if (*ptmp == ';' || *ptmp == '��' || *ptmp == '\n'
					|| *ptmp == '\0')//ɾ���� || *ptmp == ' '
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
				if (*ptmp == ',' || *ptmp == '��')
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
						if (*ptmp == ',' || *ptmp == ';' || *ptmp == '��'
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
			else if (*partValue == 0)//��������partValue��ַ��������0��˵�������������
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
		printf("û���ҵ�%s������\n", part);
		system("pause");
		ret = -4;
	}
	fclose(fp);
	return ret;
}

/*----------SOR��ģ��----------*/
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
				if (j < pre[0])//��һ���缫��ߵ������������
				{
					h1 = (dat->zi)[0] / (dat->Ni)[0];
					h2 = h1;
					break;
				}
				else if (j == pre[k] && i != 1)//�缫�߽�
				{
					h1 = (dat->zi)[k] / (dat->Ni)[k];
					h2 = (dat->di)[k] / (dat->Li)[k];
					break;
				}
				else if (j == next[k] && i != 1)//�缫�߽�
				{
					h1 = (dat->di)[k] / (dat->Li)[k];
					h2 = (dat->zi)[k + 1] / (dat->Ni)[k + 1];
					break;
				}
				else if (j > pre[k] && j < next[k] && i != 1)//�缫������
				{
					h1 = (dat->di)[k] / (dat->Li)[k];
					h2 = h1;
					break;
				}
				else if (j > next[(int)dat->n - 2])//���缫������Ҳ��������缫
				{
					h1 = (dat->zi)[(int)dat->n - 1] / (dat->Ni)[(int)dat->n - 1];
					h2 = (dat->zi)[(int)dat->n - 1] / (dat->Ni)[(int)dat->n - 1];
					break;
				}
				else if (j > next[k] && j < pre[k + 1])//�缫֮��
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
			if (i == M - 1)//��������ϵ�
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
			if (i == M - 1)//���ϵ�
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

/*----------�������----------*/
void InitialData(data *dat)//�ṹ���ʼ��
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

	fprintf(fpOut, "��������ļ�\n");
	ReadIni(fileName, "��", &dat->delta, &dat, &arrayLen);
	fprintf(fpOut, "��=%f\n", dat->delta);

	ReadIni(fileName, "n", &dat->n, &dat, &arrayLen);
	fprintf(fpOut, "n=%f\n", dat->n);
	datN = (int)dat->n;

	ReadIni(fileName, "zi", dat->zi, &dat, &arrayLen);
	if (datN != (int)arrayLen)
	{
		system("color B");
		printf("�缫֮��ľ���zi�ĸ�����缫��n��ƥ��,zi����Ӧ��n������ͬ\n");
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
		printf("�缫֮��Ĳ�����Ni�ĸ�����缫��n��ƥ��,Ni����Ӧ��n������ͬ\n");
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
		printf("�缫����di�ĸ�����缫��n��ƥ�䣬di����Ӧ�ȵ缫������һ��\n");
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
		printf("�缫���ֲ�����Li�ĸ�����缫��n��ƥ��,Li����Ӧ�ȵ缫������һ��\n");
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
		printf("�缫��λVi�ĸ�����缫��n��ƥ��\n");
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

	ReadIni(fileName, "��", &dat->e, &dat, &arrayLen);
	fprintf(fpOut, "��=%f\n", dat->e);

	ReadIni(fileName, "NST", &dat->NST, &dat, &arrayLen);
	fprintf(fpOut, "NST=%f\n", dat->NST);

	ReadIni(fileName, "b", dat->dV, &dat, &arrayLen);

	int bLen = arrayLen;
	if (bLen == 1)
		fprintf(fpOut, "ɨ���λ���Ϊ��%fV", dat->dV[0]);
	else for (i = 0; i < bLen; i++)
		fprintf(fpOut, "scanV%d=%f ", i + 1, dat->dV[i]);
	fprintf(fpOut, "\n\n");
	N = N + 1;

	//�糡��ʼ��
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
			if (j == 0)//����ߵĵ㼴�����ĵ�λ
				E[i][j] = 0;
			else if (j == N - 1)//���ұߣ����һ���缫��ӫ�����ϵĵ�λ
				E[i][j] = dat->Vi[(int)dat->n - 1];
			else
				for (k = 0; k < (int)dat->n - 1; k++)
				{
				if (j >= pre[k] && j <= next[k])//�缫��
				{
					E[i][j] = (dat->Vi)[k];
					break;
				}
				else if (j < pre[0])//��һ���缫����Ҳ����������
				{
					E[i][j] = (dat->Vi)[0] * (double)j / dat->Ni[0];
					break;
				}
				else if (j > next[(int)dat->n - 2])//���缫������Ҳ��������缫
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
	/*for (i = 0; i < M; i++)//��ʼ��������λ�������
	{
	for (j = 0; j < N; j++)
	{
	printf("%.2f ", E[i][j]);
	}
	printf("\n");
	}
	printf("\n");
	system("pause");*/

	//�̶�w
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
	fprintf(fpOut, "���w��������Ϊ12�ε� %d ��\n", time);
	time = 0;

	//��������λ
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
	fprintf(fpOut, "�������ɹ���\n");
	fprintf(fpOut, "��������Ϊ %d ��\n", time);
	fprintf(fpOut, "�����˳�ʱƽ���в�Ϊ%f�����в�Ϊ%f\n\n", canAvg, maxCan);
	fprintf(fpOut, "�����ǵ���������������ϵĵ�λֵ:\n");
	fprintf(fpOut, "���е�0���ǵ缫���ڵı߽�:\n");
	for (i = 0; i < M;)//�������ո����λ�������
	{
		fprintf(fpOut, "��%d�У�", i);
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
	if (bLen == 1)//���ֻ��һ��Ҫ�����ĵ�λֵ
		for (b = dat->dV[0]; b < 100; b = b + dat->dV[0])
		{
		setcolor(RGB(255 - 2 * b, b, 2 * b));
		printLine(M, N, b, E, multi);
		Sleep(1);
		}
	else for (i = 0; i < bLen; i++)//�ж����λ����ֵ
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

//����в��
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

/*----------��ӡ��λֵΪb�ĵ�λ��ģ��----------*/
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
			printf("��%d��û���ҵ��õ�λ %.2f V\n", i, b);
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
		for (j = 0; j < N - 1; j++)//��ɨ��
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

	for (flag = 0; flag < maxFlagX; flag++)//��������
	{
		for (i = 0; i < k[flag]; i++)
		{
			minD = M*M*multi*multi + N*N*multi*multi;
			minX = M*multi;
			minY = M*multi;
			for (j = flag; j < maxFlagX; j++)//��flag��ǰ�鿪ʼ�ң�����ǰ���ҵ�һ�»ᵼ�¶ϵ�
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

	for (flag = maxFlagX; flag < maxFlagX + maxFlagY; flag++)//��������
	{
		for (i = 0; i < k[flag]; i++)//��һ��������һ���㲻��
		{
			minD = M*M*multi*multi + N*N*multi*multi;
			minX = 0;
			minY = 0;
			for (j = flag; j < maxFlagX + maxFlagY; j++)//j=maxFlagX
			{
				for (m = 0; m < k[j]; m++)
				{
					if (j == flag&&m == i)
						break;//break��ʾ��֮ǰ�ĵ�����
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
			if (minD < 3 * multi*multi)//�����벻��̫���ʱ�������
				line(lineX[flag][i], lineY[flag][i], minX, minY);
		}
	}

	for (flag = maxFlagX; flag < maxFlagX + maxFlagY; flag++)//�������������
	{
		for (i = 0; i < k[flag]; i++)//��һ��������һ���㲻��
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
			if (minD < 3 * multi*multi)//�����벻��̫���ʱ�������
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
