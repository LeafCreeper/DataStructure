// src/quicksort.h
#pragma once
#include <cstdlib>
#include <vector>
#include <algorithm>

template <typename T>
int Partition(std::vector<T> &A, int p, int r){
    // // 初始化：选取首、尾、中间的中位数
    // int mid = p + (r - p) / 2;
    // int pivot = ((A[p] <= A[mid] && A[mid] <= A[r]) || (A[r] <= A[mid] && A[mid] <= A[p])) ? mid : (((A[mid] <= A[p] && A[p] <= A[r]) || (A[r] <= A[p] && A[p] <= A[mid])) ? p : r);
    // std::swap(A[pivot],A[r]); // 把我选定的基准交换到尾部，这样就无需修改其他的分区逻辑

    // Partition逻辑主体
    T x = A[r];
    int i = p-1;
    for(int j = p; j < r; j++){
        if (A[j]<x){
            i++;
            std::swap(A[i],A[j]);
        }
    }
    std::swap(A[i+1],A[r]);
    return i+1;
}

template <typename T>
void quickSort(std::vector<T> &A, int p = 0, int r = -1){
    if (p >= r) return;
    // 这个边界检查让我他妈的debug了好久啊。。如果不加这个边界检查，当Partition返回了q=p时就会出错，会随机爆雷
    // 最小的攻击性反例是{1,0}
    // 主要是，这和我的r==-1默认值->排序整个数组正好撞上了
    if(r == -1)r=A.size()-1;
    if(p < r){
        int q = Partition(A, p, r);
        quickSort(A, p, q-1);
        quickSort(A, q+1, r);
    }
    return;
}

template<typename T>
// 统一一个签名
void quickSort(std::vector<T> &A){
    quickSort(A, 0, -1);
    return;
}