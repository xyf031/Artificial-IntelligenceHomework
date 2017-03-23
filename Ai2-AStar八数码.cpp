//
//  main.cpp
//  AiHomeWork02C
//
//  Created by XYF on 14-11-27.
//  Copyright (c) 2014年 XYF. All rights reserved.
//

#include <iostream>
#include <list>
using namespace std;

const int GOAL[] = {2, 1, 3, 8, 0, 4, 7, 6, 5};

class node {
public:
    int data[9];
    node *path = NULL;
    int g, h, f;
};

list<node> tree; //A-Star算法节点树，保存所有已知节点及其信息，严格要求内部指针结构。
list<node> open; //open表，存data数组即可，f、g、h和指针信息无所谓。
list<node> closed; //closed表，存data数组即可，f、g、h和指针信息无所谓。


bool arrayIsEqual(const int a[9], const int b[9])
{
    for (int i=0; i<9; i++)
        if (a[i] != b[i])
            return false;
    
    return true;
}


bool comp(const node a, const node b)
{
    //open集合元素进行排序时使用sort函数，调用本函数
    if (a.f < b.f)
        return true;
    else
        return false;
}


bool comp1(const node a, const node b)
{
    //closed集合元素进行去重复时使用unique函数，调用本函数
    if (arrayIsEqual(a.data, b.data))
        return true;
    else
        return false;
}


bool contain(const node n, list<node> v)
{
    if (v.size() <= 0) {
        return false;
    }
    
    list<node>::iterator i;
    for (i = v.begin(); i != v.end(); i++) {
        if (arrayIsEqual(i->data, n.data)) {
            return true;
        }
    }
    return false;
}


void addPath(node n, node* &p)
{
    for (list<node>::iterator ii = tree.begin(); ii != tree.end(); ii++) {
        if (arrayIsEqual(n.data, ii->data)) {
            p = &(*ii);
            return;
        }
    }
}

int h(node n)
{
    //h(n)函数，表示不在目标位置的牌数
    int sum = 0;
    for (int i=0; i<9; i++) {
        if (n.data[i]!=0 && n.data[i]!=GOAL[i]) {
            sum += 1;
        }
    }
    return sum;
}


void printPath(const node n, int step)
{
    if (n.path == NULL) {
        cout << step << endl << endl;
        return;
    }
    
    printPath(*n.path, step+1);
    cout << n.data[0] << " ";
    cout << n.data[1] << " ";
    cout << n.data[2] << endl;
    cout << n.data[3] << " ";
    cout << n.data[4] << " ";
    cout << n.data[5] << endl;
    cout << n.data[6] << " ";
    cout << n.data[7] << " ";
    cout << n.data[8] << endl << endl;
    
}


void getNext(list<node> *v, node n);
void expand(node n)
{
    list<node> next;
    node temp;
    
    getNext(&next, n);
    
    for (list<node>::iterator ii = next.begin(); ii != next.end(); ii++) {
        temp = *ii;
        
        if (contain(temp, open)) {
            for (list<node>::iterator ii = tree.begin(); ii != tree.end(); ii++) {
                if (ii->g > n.g+1 && arrayIsEqual(ii->data, temp.data)) {
                    ii->g = n.g + 1;
                    ii->f = ii->g + ii->h;
                    addPath(n, ii->path);
                    break;
                }
            }
        }else if (contain(temp, closed))
        {
            for (list<node>::iterator ii = tree.begin(); ii != tree.end(); ii++) {
                if (ii->g > n.g+1 && arrayIsEqual(ii->data, temp.data)) {
                    ii->g = n.g + 1;
                    ii->f = ii->g + ii->h;
                    addPath(n, ii->path);
                    open.push_front(*ii);
                    break;
                }
            }
            
        }else{
            //tree一定含n，而一定不含temp。
            temp.g = n.g + 1;
            temp.h = h(temp);
            temp.f = temp.g + temp.h;
            addPath(n, temp.path);
            tree.push_front(temp);
            open.push_front(temp);
        }
    }
}

void Astar()
{
    //The initial value.
    node start;
    start.data[0] = 1;
    start.data[1] = 3;
    start.data[2] = 4;
    start.data[3] = 8;
    start.data[4] = 2;
    start.data[5] = 5;
    start.data[6] = 7;
    start.data[7] = 0;
    start.data[8] = 6;
    
    start.g = 0;
    start.h = h(start);
    start.f = start.g + start.h;
    open.insert(open.begin(), start);
    
    
    int i=0;
    //A-Star algorithm begins.
    node temp;
    while (!arrayIsEqual(open.front().data, GOAL))
    {
        temp = open.front();
        open.erase(open.begin());
        
        if (!contain(temp, tree)) {
            tree.push_front(temp);
        }
        
        closed.push_front(temp);
        closed.unique(comp1); //这一步不必要
        
        expand(temp);
        
        if (open.size() == 0) {
            cout << "No Solution." << endl;
            return;
        }
        
        open.sort(comp);
        
        //
        if (++i % 1000 == 0) {
            cout << i << ": " << tree.size() << endl;
        }
    }
    
    printPath(open.front(), 0);
}


int main(int argc, const char * argv[]) {
    
    Astar();
    return 0;
}


void getNext(list<node> *v, node n)
{
    //依具体问题变化。生成下一步可以走到的所有状态。
    
    if (n.data[0] == 0) {
        node temp1, temp2;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = n.data[i];
        }
        temp1.data[0] = temp1.data[1];
        temp1.data[1] = 0;
        temp2.data[0] = temp2.data[3];
        temp2.data[3] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        
    }else if(n.data[1] == 0) {
        node temp1, temp2, temp3;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = temp3.data[i] = n.data[i];
        }
        temp1.data[1] = temp1.data[0];
        temp1.data[0] = 0;
        temp2.data[1] = temp2.data[2];
        temp2.data[2] = 0;
        temp3.data[1] = temp3.data[4];
        temp3.data[4] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        v->push_back(temp3);
        
    }else if(n.data[2] == 0) {
        node temp1, temp2;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = n.data[i];
        }
        temp1.data[2] = temp1.data[1];
        temp1.data[1] = 0;
        temp2.data[2] = temp2.data[5];
        temp2.data[5] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        
    }else if(n.data[3] == 0) {
        node temp1, temp2, temp3;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = temp3.data[i] = n.data[i];
        }
        temp1.data[3] = temp1.data[0];
        temp1.data[0] = 0;
        temp2.data[3] = temp2.data[4];
        temp2.data[4] = 0;
        temp3.data[3] = temp3.data[6];
        temp3.data[6] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        v->push_back(temp3);
        
    }else if(n.data[4] == 0) {
        node temp1, temp2, temp3, temp4;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = temp3.data[i] = temp4.data[i] = n.data[i];
        }
        temp1.data[4] = temp1.data[1];
        temp1.data[1] = 0;
        temp2.data[4] = temp2.data[3];
        temp2.data[3] = 0;
        temp3.data[4] = temp3.data[5];
        temp3.data[5] = 0;
        temp4.data[4] = temp4.data[7];
        temp4.data[7] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        v->push_back(temp3);
        v->push_back(temp4);
        
    }else if(n.data[5] == 0) {
        node temp1, temp2, temp3;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = temp3.data[i] = n.data[i];
        }
        temp1.data[5] = temp1.data[8];
        temp1.data[8] = 0;
        temp2.data[5] = temp2.data[2];
        temp2.data[2] = 0;
        temp3.data[5] = temp3.data[4];
        temp3.data[4] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        v->push_back(temp3);
        
    }else if(n.data[6] == 0) {
        node temp1, temp2;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = n.data[i];
        }
        temp1.data[6] = temp1.data[7];
        temp1.data[7] = 0;
        temp2.data[6] = temp2.data[3];
        temp2.data[3] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        
    }else if(n.data[7] == 0) {
        node temp1, temp2, temp3;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = temp3.data[i] = n.data[i];
        }
        temp1.data[7] = temp1.data[6];
        temp1.data[6] = 0;
        temp2.data[7] = temp2.data[8];
        temp2.data[8] = 0;
        temp3.data[7] = temp3.data[4];
        temp3.data[4] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        v->push_back(temp3);
        
    }else if(n.data[8] == 0) {
        node temp1, temp2;
        for (int i=0; i<9; i++) {
            temp1.data[i] = temp2.data[i] = n.data[i];
        }
        temp1.data[8] = temp1.data[5];
        temp1.data[5] = 0;
        temp2.data[8] = temp2.data[7];
        temp2.data[7] = 0;
        v->push_back(temp1);
        v->push_back(temp2);
        
    }
}

