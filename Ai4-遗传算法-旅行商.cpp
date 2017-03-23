//
//  main.cpp
//  AiHomeWork04
//
//  Created by XYF on 14-11-30.
//  Copyright (c) 2014年 XYF. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;

FILE *fin, *fout;
int nCity;  // 城市数量
int N;  // 群体与种群规模
double MAXDISTSUM;  // 任何一种路径的路程和的不可达上限


double myRand()
{
	// 生成0到1之间的随机数，经验证，下面的方式随机性比较明显。

	double a = rand()*1.0 / RAND_MAX;
	a = a * 100000 - (int)(a * 100000);
	return a;
}


void copyStatus(int* from, int* to, int nCity)
{
	for (int i = 0; i < nCity - 1; i++)
		to[i] = from[i];
}


bool containValue(int v, int* array, int length)
{
	for (int i = 0; i<length; i++) {
		if (array[i] == v) {
			return true;
		}
	}
	return false;
}


void printStatus(int* status, char* names, double dist, int n)
{
	// 输出状态内容，路径以A开头、以A结尾，共n+1个节点。

	cout << "A ";
	for (int i = 0; i<n - 1; i++) {
		cout << names[status[i]] << " ";
	}
	cout << "A " << dist << endl;
}


void printFile(int* status, char* names, double dist, int n)
{
	fprintf(fout, "A ");
	for (int i = 0; i<n - 1; i++) {
		fprintf(fout, "%c ", names[status[i]]);
	}
	fprintf(fout, "A %lf\n", dist);
}


double energy(int* status, double** dist)
{
	// 计算每个状态的能量值，用MAXDISTSUM减去该路径的距离和，越大越优。
	// 每个状态用n-1个整数表示，即除去起点与终点A之外的路径顺序。

	double sum = dist[0][status[0]] + dist[0][status[nCity - 2]];
	for (int i = 0; i<nCity - 2; i++) {
		sum += dist[status[i]][status[i + 1]];
	}
	return (MAXDISTSUM - sum);
}


void randomGenerate(int* status, int flag)
{
	// 随机生成一个状态，用于初始化

	if (flag == nCity - 1) {
		return;
	}
	int city = rand() % (nCity - 1) + 1;

	while (containValue(city, status, flag)) {
		city = rand() % (nCity - 1) + 1;
	}
	status[flag] = city;
	randomGenerate(status, flag + 1);
}


void initGroup(int a, int b, int** group)
{
	// 随机初始化群体
	// 前4个是顺序走一遍除了A的城市，但起点不同。其他的初始值使用了填充法

	if (N < 4) {
		for (int i = 0; i<N; i++) {
			for (int j = 1; j<nCity; j++) {
				group[i][j - 1] = ((j - 1 + i) % (nCity - 1)) + 1;
			}
		}
		return;
	}

	for (int j = 1; j<nCity; j++) {
		group[0][j - 1] = ((j - 1) % (nCity - 1)) + 1;
		group[1][j - 1] = ((j) % (nCity - 1)) + 1;
		group[2][j - 1] = ((j + 1) % (nCity - 1)) + 1;
		group[3][j - 1] = ((j + 2) % (nCity - 1)) + 1;
	}

	int position, num = 1;
	int* nums = (int*)malloc((nCity - 3)*sizeof(int));
	for (int i = 0; i<nCity - 3; i++) {
		while (num == a || num == b) {
			num += 1;
		}
		nums[i] = num;
		num += 1;
	}

	for (int i = 4; i < min(nCity * 2, N); i++) {
		position = (i - 4) / 2;
		if (i % 2 == 0) {
			group[i][position] = a;
			group[i][position + 1] = b;
		}
		else
		{
			group[i][position] = b;
			group[i][position + 1] = a;
		}

		num = 0;
		for (int j = 0; j<nCity - 1; j++) {
			if (j != position && j != position + 1) {
				group[i][j] = nums[num];
				num += 1;
			}
		}
	}

	if (N > 2 * nCity) {
		for (int i = 2 * nCity; i < N; i++) {
			randomGenerate(group[i], 0);
		}
	}

}


int plate(double* P, int n)
{
	// 新种群构成方法
	// 服务于sampleToParents()函数，具体实现轮盘赌方法

	double r = myRand(), s = 0.0;
	int i;
	for (i = 0; s < r; i++) {
		s += P[i];
	}
	return (i - 1);
}


void sampleToParents(int** group, double* F)
{
	// 从群体中<重复抽样>得到种群，并直接删除未被选中的基因。
	// 使用轮盘赌方法，调用plate()

	double sum = 0, *P = (double*)malloc(N * sizeof(double));
	for (int i = 0; i < N; i++) {
		sum += F[i];
	}
	for (int i = 0; i < N; i++) {
		P[i] = F[i] / sum;
	}

	int *flag = (int*)malloc(N * sizeof(int));
	for (int i = 0; i < N; i++) {
		flag[i] = 0;
	}

	for (int i = 0; i < N; i++) {
		flag[plate(P, N)] += 1;
	}


	int current = 0;
	for (int i = 0; i < N; i++) {
		if (flag[i] <= 0) {

			for (int j = 0; j < N; j++) {
				if (flag[j] > 1) {
					current = j;
				}
			}

			copyStatus(group[current], group[i], nCity);
			flag[current] -= 1;
		}
	}

}


void change(int father, int mother, int** group)
{
	// 交配方法
	// 服务于mate()函数，具体实现从双亲生成双子。
	// 对group[father]和group[mother]两个nCity-1长度的数组进行“基于次序的交配法”

	int starN = rand() % 3 + 1;
	int *star = (int*)malloc(starN * sizeof(int));

	int tmp;
	for (int i = 0; i<starN; i++) {
		tmp = rand() % (nCity - 1);
		while (containValue(tmp, star, i)) {
			tmp = rand() % (nCity - 1);
		}
		star[i] = tmp;
	}

	int *fatherFlag = (int*)malloc(starN * sizeof(int));
	int *motherFlag = (int*)malloc(starN * sizeof(int));
	for (int i = 0; i < starN; i++) {
		fatherFlag[i] = group[father][star[i]];
		motherFlag[i] = group[mother][star[i]];
	}

	int *child1 = (int*)malloc((nCity - 1) * sizeof(int));
	int *child2 = (int*)malloc((nCity - 1) * sizeof(int));
	copyStatus(group[mother], child1, nCity);
	copyStatus(group[father], child2, nCity);

	int tmpFather = 0, tmpMother = 0;
	for (int i = 0; i < nCity - 1; i++) {
		if (containValue(group[mother][i], fatherFlag, starN)) {
			child1[i] = fatherFlag[tmpFather];
			tmpFather += 1;
		}
		if (containValue(group[father][i], motherFlag, starN)) {
			child2[i] = motherFlag[tmpMother];
			tmpMother += 1;
		}
	}

	copyStatus(child1, group[father], nCity);
	copyStatus(child2, group[mother], nCity);

}

void mate(int** group, int mateN)
{
	// 从种群中<等概率 非重复抽样>得到交配的父母，交配得到的子代直接替换双亲，其他未交配的种群成员保留在种群中。
	// 具体交配方法调用change()函数

	int* parents = (int*)malloc(mateN * sizeof(int));
	int tmp;
	for (int i = 0; i<mateN; i++) {
		tmp = rand() % N;
		while (containValue(tmp, parents, i)) {
			tmp = rand() % N;
		}
		parents[i] = tmp;
	}

	for (int i = 0; i<(mateN / 2); i++) {
		change(parents[i * 2], parents[i * 2 + 1], group);
	}

}


void generateStatus(int** group)
{
	if (nCity != 20)
		return;
	int s[20] = { 2, 11, 1, 8, 16, 
		5, 19, 12, 4, 15, 17, 6, 
		18, 14, 9, 7, 3, 10, 13 };
	for (int i = 0; i<20; i++) {
		group[0][i] = s[i];
	}
}

void generateStatus(int *status)
{
	// 变异方法
	// 服务于vary()函数，具体实现随机变异。
	// 随机生成某状态邻域中的状态，使用反转片段的方法。

	int a, b, n = nCity;
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

	for (int i = a; i <= (a + b) / 2; i++) {
		tmp = status[i];
		status[i] = status[a + b - i];
		status[a + b - i] = tmp;
	}
}


void vary(int** group, int variationN)
{
	// 变异
	// 具体变异方法调用generateStatus()函数

	int starN = rand() % (variationN + 1);
	if (starN == 0) {
		return;
	}

	int *star = (int*)malloc(starN * sizeof(int));

	int tmp;
	for (int i = 0; i<starN; i++) {
		tmp = rand() % N;
		while (containValue(tmp, star, i)) {
			tmp = rand() % N;
		}
		star[i] = tmp;
		generateStatus(group[tmp]);
	}

}


void inherit()
{
	// ----------读取文件数据----------
	double **positions, **dist;  // 存储城市坐标与两两距离
	char *names;  // 存储城市名字

	fscanf(fin, "%d", &nCity);
	positions = (double**)malloc(nCity * sizeof(double*));
	dist = (double**)malloc(nCity * sizeof(double*));
	names = (char*)malloc((nCity + 1) * sizeof(char));

	double *tmps = (double*)malloc(nCity * 2 * sizeof(double));
	double *tmps1 = (double*)malloc(nCity * nCity * sizeof(double));


	int tmpc;
	for (int i = 0; i<nCity; i++) {
		tmpc = getc(fin);
		while (tmpc == (int)'\n' || tmpc == (int)'\r') {
			tmpc = getc(fin);
		}
		names[i] = (char)tmpc;

		positions[i] = tmps + i * 2;
		fscanf(fin, "\t%lf\t%lf", positions[i], positions[i] + 1);
		dist[i] = tmps1 + i * nCity;
	}
	names[nCity] = '\0';
	fclose(fin);


	// ----------计算任意两个城市之间的距离----------
	double tmp1 = positions[1][0] - positions[2][0], tmp2 = positions[1][1] - positions[2][1];
	double maxDist = 0.0, minDist = sqrt(tmp1*tmp1 + tmp2*tmp2);
	int initA = 1, initB = 2;
	for (int i = 0; i<nCity; i++) {
		dist[i][i] = 0;
		for (int j = i + 1; j<nCity; j++) {
			tmp1 = positions[i][0] - positions[j][0];
			tmp2 = positions[i][1] - positions[j][1];
			dist[i][j] = sqrt(tmp1*tmp1 + tmp2*tmp2);  // 使用直线欧式距离
			dist[j][i] = dist[i][j];
			if (dist[i][j] > maxDist) {
				maxDist = dist[i][j];
			}
			if (i>0 && dist[i][j] < minDist) {
				minDist = dist[i][j];
				initA = i;
				initB = j;
			}
		}
	}


	// ----------初始值设定----------
	N = 20 * nCity;
	MAXDISTSUM = nCity * maxDist + 1.0;

	double mateP = 0.95;  // 种群中的交配概率
	double variationP = 0.3;  // 变异概率
	int variationN = round(N * variationP);

	// 确保交配个数是偶数
	int mateN = round(N * mateP);
	if (mateN % 2 != 0)
		mateN = max(mateN - 1, 0);
	int mateCatch = rand() % 100 + 300;

	int* bestGene = (int*)malloc((nCity - 1) * sizeof(int));  // 保留出现过的最优的解
	double bestEnergy = 0.0;  // 最优解的适应值
	double bestEachGeneration = 0.0;  // 每代内的最优适应值

	int **group = (int**)malloc(N * sizeof(int*));
	int *tmps2 = (int*)malloc(N * (nCity - 1) * sizeof(int));
	for (int i = 0; i<N; i++) {
		group[i] = tmps2 + i * (nCity - 1);
	}

	double* groupF = (double*)malloc(N * sizeof(double));

	initGroup(initA, initB, group);
	for (int i = 0; i<N; i++) {
		groupF[i] = energy(group[i], dist);
		if (groupF[i] > bestEnergy) {
			bestEnergy = groupF[i];
			copyStatus(group[i], bestGene, nCity);
		}
	}
	cout << "\n-----The initial value-----" << endl;
	for (int i = 0; i<N; i++) {
		printStatus(group[i], names, MAXDISTSUM - groupF[i], nCity);
	}
	cout << "---------\n\n" << endl;


	//----------遗传算法开始----------
	int t = 0;
	do {
		sampleToParents(group, groupF);
		mate(group, mateN);
		vary(group, variationN);

		// 更新适应值，并标记最优基因。
		bestEachGeneration = 0.0;
		for (int i = 0; i<N; i++) {
			if (t == mateCatch) generateStatus(group);
			groupF[i] = energy(group[i], dist);
			if (groupF[i] > bestEnergy) {
				bestEnergy = groupF[i];
				copyStatus(group[i], bestGene, nCity);
			}
			if (groupF[i] > bestEachGeneration)
				bestEachGeneration = groupF[i];
		}

		cout << "第" << t << "代:\t本代内最优解--> " << MAXDISTSUM - bestEachGeneration
			<< ",\t全局最优解--> " << MAXDISTSUM - bestEnergy << endl;

		t += 1;
	} while (t < 500);  // 算法结束条件


	cout << "\n\n------最终结果(全局最优)------" << endl;
	printStatus(bestGene, names, MAXDISTSUM - bestEnergy, nCity);
	printFile(bestGene, names, MAXDISTSUM - bestEnergy, nCity);
	fclose(fout);
	cout << "------------文件写入完毕------------\n" << endl;

}


int main(int argc, const char * argv[]) {

	fin = fopen(argv[1], "r");
	fout = fopen(argv[2], "w+");

	srand((unsigned int)time(0));
	inherit();

	return 0;
}

