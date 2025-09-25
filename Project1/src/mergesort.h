// src/mergesort.h
#pragma once
#include <vector>

template <typename T>
void mergeSort(std::vector<T> &A, int left = 0, int right = -1){ //设置了默认值，默认排序整个部分
    if(right==-1)right=A.size()-1; //我本来想写right = A.size()，但是这在c++里不可行
    
    // 递归的边界条件
    if(right<=left)return;

    // 递归归并部分
    int mid = (right + left)/2;
    mergeSort(A, left, mid);
    mergeSort(A, mid+1, right);

    // 合并部分 (1)把两部分拆出来，单独存储
    // 用迭代器构造子数组
    std::vector<T> A_leftPart(A.begin() + left, A.begin() + mid + 1);
    std::vector<T> A_rightPart(A.begin() + mid + 1, A.begin() + right + 1);


    // 合并部分 (2)把两部分合并回原位置
    int i,j,k;
    i=j=0; k=left; //k不是从0开始的，考虑到排序区间不一定是整个数组A
    while (i < A_leftPart.size() && j < A_rightPart.size()){
        if(A_leftPart[i]>A_rightPart[j]) A[k++]=A_rightPart[j++];
        else A[k++]=A_leftPart[i++];
    }

    while(i < A_leftPart.size()) A[k++] = A_leftPart[i++];
    while(j < A_rightPart.size()) A[k++] = A_rightPart[j++];
}

// 对于递归编程，要注意考虑每一次递归调用时函数被传参的是什么。一开始误以为每一次都在排“A”，所以写错了初始条件和k的初始值；
// 回头要再写一遍