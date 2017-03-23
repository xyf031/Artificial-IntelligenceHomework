
#include <iostream>
using namespace std;

class Store {
public:
	int mild, chman, boat;
	Store *next;
};

int check(int mild, int chman, int n);
int move(int *mild, int *chman, int *boat, int n, int k, Store *history, FILE *fout);
void addToLink(Store *history, int mild, int chman, int boat);
void popLink(Store *history);
bool contain(Store *history, int mild, int chman, int boat);
void printLink(Store *history, FILE *fout);

int main(int argc, const char * argv[]) {

	int n, k;
	FILE *fin = fopen(argv[1], "r");
	fscanf(fin, "%d%d", &n, &k);
	fclose(fin);

	int mild = n, chman = n, boat = 1;
	//mild表示左岸野人数量，chman表示左岸传教士数量，boat＝1表示船在左岸。

	Store *history = (Store *)malloc(sizeof(Store));
	history->mild = n;
	history->chman = n;
	history->boat = 1;
	history->next = NULL;
	//初始状态存储

	FILE *fout = fopen(argv[2], "w+");
	if (move(&mild, &chman, &boat, n, k, history, fout) == 0)
		fprintf(fout, "No Solution.");

	fclose(fout);
	return 0;
}

int check(int mild, int chman, int n)
{
	int rmild = n - mild;
	int rchman = n - chman;

	if (mild<0 || chman<0 || rmild<0 || rchman<0)
	{
		return 0;
	}
	if (mild>chman && chman>0) {
		return 0;//左岸检测
	}
	if (rmild>rchman && rchman>0) {
		return 0;//右岸检测
	}
	return 1;
}

int move(int *mild, int *chman, int *boat, int n, int k, Store *history, FILE *fout)
{
	if (*mild == 0 && *chman == 0) {

		//将状态集合输出到文件
		printLink(history, fout);
		return 1;//
	}

	if (*boat == 1) {
		for (int i = 0; i <= k; i++) {
			for (int j = 0; j <= k - i; j++) {
				/*if的条件是：船上至少有一个人 && 船上的传教士是安全的 && 划过去之后两岸是安全的 && 划过去之后的状态是第一次出现的*/
				if (i + j>0 && (j == 0 || (i <= j && j>0)) && check(*mild - i, *chman - j, n) == 1
					&& !contain(history, *mild - i, *chman - j, 0)) {
					*mild -= i;
					*chman -= j;
					*boat = 0;
					addToLink(history, *mild, *chman, *boat);

					//开始递归调用
					if (move(mild, chman, boat, n, k, history, fout) == 0) {
						*mild += i;
						*chman += j;
						*boat = 1;
						popLink(history);
					}
					else
						return 1;//
				}
			}
		}
	}
	else
	{
		for (int i = 0; i <= k; i++) {
			for (int j = 0; j <= k - i; j++) {
				//if条件的含义同上
				if (i + j>0 && (j == 0 || (i <= j && j>0)) && check(*mild + i, *chman + j, n) == 1
					&& !contain(history, *mild + i, *chman + j, 1)) {
					*mild += i;
					*chman += j;
					*boat = 1;
					addToLink(history, *mild, *chman, *boat);

					//开始递归调用
					if (move(mild, chman, boat, n, k, history, fout) == 0) {
						*mild -= i;
						*chman -= j;
						*boat = 0;
						popLink(history);
					}
					else
						return 1;//
				}
			}
		}
	}

	return 0;//
}


//下面4个函数是自行构建的、用链表实现的历史状态存储结构
void addToLink(Store *history, int mild, int chman, int boat) {
	Store *myhistory = (Store *)malloc(sizeof(Store));
	myhistory->mild = mild;
	myhistory->chman = chman;
	myhistory->boat = boat;
	myhistory->next = NULL;

	Store *p = history;
	while (p->next != NULL) {
		p = p->next;
	}
	p->next = myhistory;
}

void popLink(Store *history) {
	Store *p = history;
	if (p->next == NULL) {
		return;
	}

	while (p->next->next != NULL) {
		p = p->next;
	}
	p->next = NULL;
}

bool contain(Store *history, int mild, int chman, int boat) {
	Store *p = history;
	while (p != NULL) {
		if (p->mild == mild && p->chman == chman && p->boat == boat)
			return true;
		p = p->next;
	}
	return false;
}

void printLink(Store *history, FILE *fout)
{
	Store *p = history;
	while (p != NULL) {
		fprintf(fout, "(%d,%d,%d)\n", p->mild, p->chman, p->boat);
		p = p->next;
	}
}

