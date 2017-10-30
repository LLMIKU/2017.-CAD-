#include "head.h"

void InitialData(data *dat)
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
	dat->dV = 0;
}

void main()
{
	double **E = NULL;
	int i, j, k;
	int M = 0, N = 0;

	int arrayLen = 0;
	int datN = 0;
	data *dat = (data *)malloc(sizeof(data));
	InitialData(dat);

	ReadIni("lqData.dat", "δ", &dat->delta, &dat, &arrayLen);
	printf("δ=%f\n", dat->delta);

	ReadIni("lqData.dat", "n", &dat->n, &dat, &arrayLen);
	printf("n=%f\n", dat->n);
	datN = (int)dat->n;

	ReadIni("lqData.dat", "zi", dat->zi, &dat, &arrayLen);
	for (i = 0; i < arrayLen; i++)
	{
		printf("z%d=%f\n", i + 1, (dat->zi)[i]);
	}
	if (datN != (int)arrayLen)
	{
		system("color B");
		printf("电极之间的距离zi的个数与电极数n不匹配,zi个数应和n个数相同\n");
		system("pause");
		return;
	}

	ReadIni("lqData.dat", "Ni", dat->Ni, &dat, &arrayLen);
	for (i = 0; i < arrayLen; i++)
	{
		printf("N%d=%f ", i + 1, (dat->Ni)[i]);
		N += (int)(dat->Ni)[i];
	}
	printf("\n");
	if (datN != (int)arrayLen)
	{
		system("color B");
		printf("电极之间的步长数Ni的个数与电极数n不匹配,Ni个数应和n个数相同\n");
		system("pause");
		return;
	}

	ReadIni("lqData.dat", "di", dat->di, &dat, &arrayLen);
	for (i = 0; i < arrayLen; i++)
	{
		printf("d%d=%f ", i + 1, (dat->di)[i]);
	}
	printf("\n");
	if (datN != (int)(arrayLen + 1))
	{
		system("color B");
		printf("电极长度di的个数与电极数n不匹配，di个数应比电极个数少一个\n");
		system("pause");
		return;
	}

	ReadIni("lqData.dat", "Li", dat->Li, &dat, &arrayLen);
	for (i = 0; i < arrayLen; i++)
	{
		printf("L%d=%f ", i + 1, (dat->Li)[i]);
		N += (int)(dat->Li)[i];
	}
	printf("\n");
	if (datN != (int)(arrayLen + 1))
	{
		system("color B");
		printf("电极划分步长数Li的个数与电极数n不匹配,Li个数应比电极个数少一个\n");
		system("pause");
		return;
	}

	ReadIni("lqData.dat", "Vi", dat->Vi, &dat, &arrayLen);
	for (i = 0; i < arrayLen; i++)
	{
		printf("Vi=%f ", (dat->Vi)[i]);
	}
	printf("\n");
	if (datN != (int)(arrayLen))
	{
		system("color B");
		printf("电极电位Vi的个数与电极数n不匹配\n");
		system("pause");
		return;
	}

	ReadIni("lqData.dat", "r", &dat->r, &dat, &arrayLen);
	printf("r=%f\n", dat->r);
	ReadIni("lqData.dat", "M", &dat->M, &dat, &arrayLen);
	printf("M=%f\n", dat->M);
	M = (int)dat->M + 2;
	ReadIni("lqData.dat", "ε", &dat->e, &dat, &arrayLen);
	printf("ε=%f\n", dat->e);
	ReadIni("lqData.dat", "NST", &dat->NST, &dat, &arrayLen);
	printf("NST=%f\n", dat->NST);
	ReadIni("lqData.dat", "b", &dat->dV, &dat, &arrayLen);
	printf("dV=%f\n", dat->dV);
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
	/*for (i = 0; i < M; i++)//各点电位输出测试
	{
		for (j = 0; j < N; j++)
		{
			printf("%.2f ", E[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	system("pause");*/
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
		w = 2 / (1 + sqrt(1 - u*u));
		w = 1.25*w - 0.5;
		canAll = 0;
		canAllTmp = 0;
		limit = fabs(w - wTmp) / fabs(2 - w);
		time++;
		if (limit <= 0.05)
			break;
	}
	printf("w is %f\n", w);
	printf("求解w迭代次数为12次的 %d 倍\n", time);
	system("pause");
	time = 0;

	while (eFlag)
	{
		eFlag = 0;
		time++;
		sor(M, N, dat, E, can, &w, &canCount, pre, next);
		for (i = 0; i < canCount - 1; i++)
		{
			if (can[i]>dat->e)
			{
				eFlag = 1;
				break;
			}	
		}
	}
	printf("迭代求解成功！\n");
	printf("迭代次数为 %d 次\n", time);
	system("pause");
	
	/*for (i = 0; i < M; i++)//各点电位输出测试
	{
		for (j = 0; j < N; j++)
		{
			printf("%.2f ", E[i][j]);
		}
		printf("\n");
	}
	system("pause");
	*/
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
	for (b = dat->dV; b < 200; b = b + dat->dV,i++)
	{
		/*if (i % 7 == 6)//彩虹色等位线测试
			setcolor(RGB(255, 0, 0));
		else if (i % 7 == 5)
			setcolor(RGB(255, 165, 0));
		else if (i % 7 == 4)
			setcolor(RGB(255, 255, 0));
		else if (i % 7 == 3)
			setcolor(RGB(0, 255, 0));
		else if (i % 7 == 2)
			setcolor(RGB(0, 127, 255));
		else if (i % 7 == 1)
			setcolor(RGB(0, 0, 255));
		else if (i % 7 == 0)
			setcolor(RGB(139, 0, 255));*/
		setcolor(RGB(100, 2*b ,  0));
		printLine(M, N, b, E, multi); 
		Sleep(1);
	}
	
	/*printLine(M, N, 70, E, multi);//测试用例*/
	
	
	for (i = 0; i < M; i++)
	{
		free(E[i]);
	}
	free(E);
	free(pre);
	free(next);
	free(can);
	exit(0);
}

double calCan(int canCount,double *can)
{
	double canAll = 0;
	int i;
	for (i = 0; i < canCount - 1; i++)
	{
		canAll += can[i];
	}
	return canAll;
}

int printLine(int M,int N,double b,double **E,int multi)
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
		if (iFlag == 0)
		{
			printf("第%d行没有找到该电位 %.2f V\n", i, b);
			system("pause");
		}	
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

	int **lineX = (int **)malloc((maxFlagX+maxFlagY)*sizeof(int *));
	int **lineY = (int **)malloc((maxFlagX+maxFlagY)*sizeof(int *));
	for (i = 0; i < maxFlagX+maxFlagY; i++)
	{
		lineX[i] = (int *)malloc((M+N)*sizeof(int));
		memset(lineX[i], 0, (M + N)*sizeof(int));
	}
	for (i = 0; i < maxFlagX + maxFlagY; i++)
	{
		lineY[i] = (int *)malloc((M + N)*sizeof(int));
		memset(lineY[i], 0, (M + N)*sizeof(int));
	}
	int *k = (int *)malloc((maxFlagX+maxFlagY)*sizeof(int));
	memset(k, 0, (maxFlagX+maxFlagY)*sizeof(int));
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
				if (check(checki, checkj,multi,maxFlagX,lineX,lineY,k))
				{
					lineX[flag][k[flag]] = j * multi;
					lineY[flag][k[flag]] = i * multi;
					k[flag]++;
					flag++;
				}
			}
			else if ((E[i][j] < b && E[i+1][j] > b) || (E[i][j] > b && E[i+1][j] < b))
			{
				checki = i * multi +
					(int)((b - E[i][j]) / (E[i+1][j] - E[i][j]) * multi);
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
