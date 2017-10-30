#include "head.h"
int sor(int M,int N,data *dat,double **E,double *can,
	double *InW,int *InCanCount,int *pre,int *next)
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