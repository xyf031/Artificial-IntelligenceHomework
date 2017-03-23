# coding=utf-8
__author__ = 'XYF'

import os

#----------读取文件目录----------
tmp1 = os.popen('ls MyData/Train')
tmp1 = tmp1.read().split()
ls1 = []  # Train文件夹下的一级文件目录，代表有多少类别。
ls2 = []  # 每个ls1下面的二级文件目录，代表每个类别下面有多少文章。
for i in tmp1:
    ls1.append(i)
    tmp2 = os.popen('ls MyData/Train/' + str(i)).read().split()
    ls2.append(tmp2)


#----------各类别先验概率----------
countCk = [len(i) for i in ls2]  # 每个类别下的训练文章个数
countTotal = sum(countCk)  # 总的训练样本量
probCk = [i / countTotal for i in countCk]  # 每个类别的先验概率P(c_k)


#----------分词处理与训练Dictionary----------
def wordPerfectHead(head, word):
    if head < len(word) and word[head] in punctuations:
        head = wordPerfectHead(head + 1, word)
        return head
    else:
        return head - 1


def wordPerfectTail(tail, word):
    if tail <= len(word) and word[-tail] in punctuations:
        tail = wordPerfectTail(tail + 1, word)
        return tail
    else:
        return tail - 1

punctuations = ',.;:()[]{}|<>`~!?#%^&*_-=+"/\\\''  # 要剔除的标点符号
allFrequence = []  # list，长度是类别个数，元素类型dictionary。各类别下的训练样本单词频数统计
allProb = []  # list，长度是类别个数，元素类型dictionary。各类别下的单词对应条件概率
vocabulary = {}  #
lamda = 1
expansion = 1000
fDict = open('Dictionary.txt', 'w')
fDict.writelines('The lambda is: ' + str(lamda) + '\r\nThe expansion is: ' + str(expansion) + '\r\n\r\n\r\n')
fDict.writelines('-----------Frequence----------\r\n')

#-----统计各类别内的词频
for i in range(0, len(ls1)):
    frequence = {}
    for j in range(0, len(ls2[i])):
        read = open('MyData/Train/' + ls1[i] + '/' + ls2[i][j]).read().split()
        for i1 in range(0, len(read)):
            cutHead = wordPerfectHead(0, read[i1])
            cutTail = wordPerfectTail(1, read[i1])
            tmp1 = read[i1]
            if cutHead > -1:
                tmp1 = read[i1][cutHead + 1:]
            if len(tmp1) > 0 and cutTail > 0:
                tmp1 = tmp1[:-cutTail]
            if len(tmp1) > 0:
                if tmp1.lower() in frequence:
                    frequence[tmp1.lower()] += 1
                else:
                    frequence[tmp1.lower()] = 1

    #-----可以在这里再对单词进行过滤
    frequence2 = {}
    for i2 in frequence:
        if not '@' in str(i2) and not str(i2).isnumeric() and len(str(i2)) > 1:
            frequence2[i2] = frequence[i2]
            vocabulary[i2] = 0
    allFrequence.append(frequence2)

    fDict.writelines(ls1[i] + '\r\n')
    fDict.writelines('The words BEFORE filter is: ' + str(len(frequence)) + '\r\n')
    fDict.writelines('The words AFTER filter is: ' + str(len(frequence2)) + '\r\n\r\n')
    print('\n' + ls1[i])
    print('The words in frequence 1 is: ' + str(len(frequence)))
    print('The words in frequence 2 is: ' + str(len(frequence2)))

#-----拼合各类别的词库，并计算条件概率
fDict.writelines('\r\n-----------Probability----------\r\n')
for i in range(0, len(ls1)):
    probability = {}
    for j in vocabulary:
        if j in allFrequence[i]:
            probability[j] = expansion * (allFrequence[i][j] + lamda) / (sum(allFrequence[i].values())
                                                                         + lamda * len(vocabulary))
        else:
            probability[j] = expansion * lamda / (sum(allFrequence[i].values()) + lamda * len(vocabulary))
    allProb.append(probability)

    fDict.writelines(ls1[i] + '\r\n')
    fDict.writelines('The max probability of this cluster is: ' + str(max(probability.values())) + '\r\n')
    fDict.writelines('The min probability of this cluster is: ' + str(min(probability.values())) + '\r\n\r\n')
    print('\n' + ls1[i])
    print(sum(probability.values()))
    print('The max probability of this cluster is: ' + str(max(probability.values())))
    print('The min probability of this cluster is: ' + str(min(probability.values())))

#-----把词典写入文件-----
keys = []
for i in vocabulary:
    keys.append(str(i))
keys.sort()
fDict.writelines('\r\n-----------Dictionary----------\r\n')
fDict.writelines('Word\t\tFrequence\tProbability\t...\r\n\r\n')
for i in keys:
    fDict.writelines(str(i) + '\t\t')
    for j in range(0, len(ls1)):
        if i in allFrequence[j]:
            fDict.writelines(str(allFrequence[j][i]) + '\t' + str(allProb[j][i]) + '\t')
        else:
            fDict.writelines('#\t' + str(allProb[j][i]) + '\t')
    fDict.writelines('\r\n')

fDict.close()


#----------开始估计----------
def classify(lines):
    word = []
    for ii in lines:
        cut_head = wordPerfectHead(0, ii)
        cut_tail = wordPerfectTail(1, ii)
        temp = ii
        if cut_head > -1:
            temp = temp[cut_head + 1:]
        if len(temp) > 0 and cut_tail > 0:
            temp = temp[: -cut_tail]
        if len(temp) > 0:
            word.append(temp.lower())

    probs = [1] * len(ls1)
    for ii in range(0, len(ls1)):
        for jj in word:
            if jj in allProb[ii]:
                probs[ii] *= allProb[ii][jj]
        probs[ii] *= probCk[ii]
    return probs


def findMax(num):
    temp = [0, num[0]]
    for ii in range(1, len(num)):
        if num[ii] > temp[1]:
            temp[0] = ii
            temp[1] = num[ii]
    return temp[0]

#-----对Train训练数据集进行分类
result0 = {}
result1 = {}
correctRate0 = 0
fResultTrain = open('ResultTrain.txt', 'w')
for i in range(0, len(ls1)):
    for j in range(0, len(ls2[i])):
        read = open('MyData/Train/' + ls1[i] + '/' + ls2[i][j]).read().split()
        tmp1 = classify(read)
        tmp2 = findMax(tmp1)
        result0[(i, j)] = tmp1
        result1[(i, j)] = tmp2
        if tmp2 == i:
            correctRate0 += 1
        print(str(i) + '\t:\t' + str(tmp2) + '\t:\t' + str(tmp1))
        fResultTrain.writelines(str(i) + '(' + str(ls2[i][j]) + '):\t' + str(tmp2) + ';\t' + str(tmp1) + '\r\n')
correctRate0 /= (len(ls1) * len(ls2[0]))
fResultTrain.writelines('\r\nThe correct rate is: ' + str(correctRate0) + '\r\n')
print(correctRate0)
fResultTrain.close()

#-----对Test测试数据集进行分类
result2 = []
result3 = []
correctRate1 = 0
fResultTest = open('ResultTest.txt', 'w')

tmp1 = os.popen('ls MyData/Test')
test = tmp1.read().split()
for i in range(0, len(test)):
    read = open('MyData/Test/' + str(test[i])).read().split()
    tmp1 = classify(read)
    tmp2 = findMax(tmp1)
    result2.append(tmp1)
    result3.append(tmp2)
    if tmp2 == int(test[i][1]) - 1:
        correctRate1 += 1
    print(str(test[i][1]) + '\t:\t' + str(tmp2+1) + '\t:\t' + str(tmp1))
    fResultTest.writelines(str(test[i]) + ':\t' + str(tmp2+1) + ';\t' + str(tmp1) + '\r\n')
correctRate1 /= len(test)
print(correctRate1)
fResultTest.writelines('\r\nThe correct rate is: ' + str(correctRate1) + '\r\n')
fResultTest.close()
