//
//  main.cpp
//  AiHomeWork03
//
//  Created by XYF on 14-11-21.
//  Copyright (c) 2014年 XYF. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

FILE *fin, *fout;

double myRand()
{
	//生成0到1之间的随机数，经验证，下面的方式随机性比较明显。

	double a = rand()*1.0 / RAND_MAX;
	a = a * 100000 - (int)(a * 100000);
	return a;
}


void copyStatus(int* from, int* to, int n)
{
	for (int i = 0; i<n - 1; i++) {
		to[i] = from[i];
	}
}


double energy(int* status, int n, double** dist)
{
	//计算每个状态的能量值，用路径之和表示，越小越接近最优解。
	//每个状态用n-1个整数表示，即除去起点与终点A之外的路径顺序。

	double sum = dist[0][status[0]] + dist[0][status[n - 2]];
	for (int i = 0; i<n - 2; i++) {
		sum += dist[status[i]][status[i + 1]];
	}
	return sum;
}


void generateStatus(int *status, int n)
{
	//随机生成某状态邻域中的状态，使用反转片段的方法。

	int a, b;
	a = rand() % (n - 1);
	b = rand() % (n - 1);
	while (b == a) {
		b = rand() % (n - 1);
	}

	int tmp;
	if (a > b) {
		tmp = a;
		a = b;
		b = tmp;
	}

	//cout << "The Change Position: " << a << " & " << b << "" << endl;

	for (int i = a; i <= (a + b) / 2; i++) {
		tmp = status[i];
		status[i] = status[a + b - i];
		status[a + b - i] = tmp;
	}
}


void printStatus(int* status, char* names, double dist, int n)
{
	//在控制台输出状态内容，路径以A开头、以A结尾，共n+1个节点。

	cout << "A ";
	for (int i = 0; i<n - 1; i++) {
		cout << names[status[i]] << " ";
	}
	cout << "A " << dist << endl;
}

void printFile(int* status, char* names, double dist, int n)
{
	//将状态输出到文件。

	fprintf(fout, "A ");
	for (int i = 0; i<n - 1; i++) {
		fprintf(fout, "%c ", names[status[i]]);
	}
	fprintf(fout, "A %lf\n", dist);
}

void SA()
{
	int n; //城市数
	double **positions, **dist; //存储城市坐标与两两距离
	char *names; //存储城市名字

	fscanf(fin, "%d", &n); //getc(fin);getc(fin);
	positions = (double**)malloc(n * sizeof(double*));
	dist = (double**)malloc(n * sizeof(double*));
	names = (char*)malloc((n+1) * sizeof(char));

	double *tmps = (double*)malloc(n * 2 * sizeof(double));
	double *tmps1 = (double*)malloc(n * n * sizeof(double));


	int tmpc;
	for (int i = 0; i<n; i++) {

		tmpc = getc(fin);
		while (tmpc == (int)'\n' || tmpc == (int)'\r') {
			tmpc = getc(fin);
		}
		names[i] = (char)tmpc;

		positions[i] = tmps + i * 2;
		fscanf(fin, "\t%lf\t%lf", positions[i], positions[i] + 1); //getc(fin);getc(fin);
		dist[i] = tmps1 + i * n;
	}
	names[n] = '\0';
	fclose(fin);

	//计算任意两个城市之间的距离
	double tmp1, tmp2, maxDist = 0.0;
	for (int i = 0; i<n; i++) {
		dist[i][i] = 0;
		for (int j = i + 1; j<n; j++) {
			tmp1 = positions[i][0] - positions[j][0];
			tmp2 = positions[i][1] - positions[j][1];
			dist[i][j] = sqrt(tmp1*tmp1 + tmp2*tmp2); //使用直线欧式距离
			dist[j][i] = dist[i][j];
			if (dist[i][j] > maxDist) {
				maxDist = dist[i][j];
			}
		}
	}

	//初始值设定
	int *best = (int*)malloc(sizeof(int)*(n - 1));
	int tmps2[] = { 2, 1, 9, 8, 7, 6, 5, 4, 3 };
	int tmps3[] = { 2, 11, 1, 8, 16, 5, 19, 12, 4, 15, 17, 6, 18, 14, 9, 7, 3, 10, 13 };

	if (n == 10)
		for (int i = 1; i<n; i++)
			best[i - 1] = tmps2[i - 1];
	else if (n == 20)
		for (int i = 1; i<n; i++)
			best[i - 1] = tmps3[i - 1];
	else
		for (int i = 1; i<n; i++)
			best[i - 1] = i;


	double bestDist = energy(best, n, dist);

	double temperature = n * maxDist; //初始温度的设定

	int k = 0;
	double tmpDist, lastDist, flag = bestDist, p;
	int *tmpStatus = (int*)malloc(sizeof(int)*(n - 1));
	int *flagStatus = (int*)malloc(sizeof(int)*(n - 1));
	copyStatus(best, flagStatus, n);
	

	srand((unsigned int)time(0));
	//模拟退火法搜索开始
	do
	{
		lastDist = bestDist;

		//某温度内循环开始，以固定迭代次数为限，界限是20*n
		for (int i = 0; i<20 * n; i++) {

			//cout << "\n温度内循环次数：" << i << endl;

			//由当前最佳路径best随机生成邻域状态tmpstatus
			copyStatus(best, tmpStatus, n);

			/////
			//cout << "Before Gener: ";
			//printStatus(tmpStatus, names, bestDist, n);
			/////

			generateStatus(tmpStatus, n);

			//判断是否接受状态转移
			tmpDist = energy(tmpStatus, n, dist);

			/*//////
			cout << "After Gener: ";
			printStatus(tmpStatus, names, tmpDist, n);
			*//////

			if (tmpDist < flag) {
				flag = tmpDist;
				copyStatus(tmpStatus, flagStatus, n);
			}

			if (tmpDist <= bestDist) {
				copyStatus(tmpStatus, best, n);
				bestDist = tmpDist;
			}
			else{
				p = exp((bestDist - tmpDist) / temperature);
				//cout << "p: " << p << endl;//
				if (p > myRand()) {
					copyStatus(tmpStatus, best, n);
					bestDist = tmpDist;
					//cout << "Accepted.-----------当前温度: " << temperature << endl;
				}
			}
		}

		//温度递减
		temperature *= 0.95;
		k += 1;
		printFile(best, names, bestDist, n);

		/*/
		cout << endl << "***********" << k << "\n当前温度: " << temperature << endl;
		cout << "不同温度迭代次数: " << k << endl;
		cout << "上次温度完成时的距离、当前温度下的距离: " << lastDist << " " << bestDist << endl;
		cout << "******" << endl << endl;
		*////

	} while (temperature > 0.03 && (k < 100 || lastDist != flag || bestDist != flag));
	//大循环结束条件

	printFile(flagStatus, names, flag, n);
	cout << "------------文件写入完毕------------" << endl;
}

int main(int argc, const char * argv[]) {

	fin = fopen(argv[1], "r");
	fout = fopen(argv[2], "w+");

	SA();

	fclose(fout);
	return 0;
}
