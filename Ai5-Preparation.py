# coding=utf-8
__author__ = 'XYF'

import os
import random

tmp1 = os.popen('ls data')
tmp1 = tmp1.read().split()
ls1 = []
ls2 = []
for i in tmp1:
    ls1.append(i)
    tmp2 = os.popen('ls data/' + str(i)).read().split()
    ls2.append(tmp2)


# 需要提前新建文件夹：/MyData/Train/c1, /MyData/Train/c2, /MyData/Train/c3, /MyData/Train/c4, 以及 /MyData/Test
classes = ['Test', 'Train']
isTrain = []  # 标记是训练数据集or测试数据集
for i in range(0, len(ls1)):
    isTrain.append([1] * len(ls2[i]))
    tmp1 = round(0.2 * len(ls2[i]))  # 交叉验证数据的比例20%
    while tmp1 > 0:
        tmp2 = random.randint(0, len(ls2[i]) - 1)
        if isTrain[i][tmp2] == 1:
            isTrain[i][tmp2] = 0
            tmp1 -= 1

for i in range(0, len(ls1)):
    for j in range(0, len(ls2[i])):
        read = open('data/' + ls1[i] + '/' + ls2[i][j])
        if isTrain[i][j] == 1:
            write = open('MyData/' + classes[isTrain[i][j]] + '/c' + str(i + 1) + '/c' + str(i + 1)
                         + '_' + ls2[i][j] + '.txt', 'w+')
        else:
            write = open('MyData/' + classes[isTrain[i][j]] + '/c' + str(i + 1) + '_' + ls2[i][j] + '.txt', 'w+')
        tmp1 = read.readlines()
        read.close()
        flag = 0
        for k in tmp1:
            if flag == 1:
                write.writelines(k)
            else:
                if 'Lines:' in k:  # 切除每个文件的头信息
                    flag = 1
        write.close()

# 输出文件列表
tmp1 = os.popen('ls MyData/Train')
tmp1 = tmp1.read().split()
ls1 = []  # Train文件夹下的一级文件目录，代表有多少类别。
ls2 = []  # 每个ls1下面的二级文件目录，代表每个类别下面有多少文章。
for i in tmp1:
    ls1.append(i)
    tmp2 = os.popen('ls MyData/Train/' + str(i)).read().split()
    ls2.append(tmp2)

fTrain = open('Train List.txt', 'w')
for i in range(0, len(ls1)):
    fTrain.writelines(ls1[i] + '\r\n')
    for j in ls2[i]:
        fTrain.writelines(j[3:-4] + '\r\n')
    fTrain.writelines('\r\n')
fTrain.close()

tmp1 = os.popen('ls MyData/Test')
tmp1 = tmp1.read().split()
tmp2 = {}
fTest = open('Test List.txt', 'w')
for i in tmp1:
    if i[0:2] in tmp2:
        tmp2[i[0:2]].append(i[3:-4])
    else:
        tmp2[i[0:2]] = [i[3:-4]]

for i in tmp2.keys():
    fTest.writelines(i + '\r\n')
    for j in tmp2[i]:
        fTest.writelines(j + '\r\n')
    fTest.writelines('\r\n')
fTest.close()
