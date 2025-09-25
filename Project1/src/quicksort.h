// // src/quicksort.h
// #pragma once
// #include <cstdlib>
// #include <vector>
// #include <algorithm>

// template <typename T>
// int Partition(std::vector<T> &A, int p, int r){
//     // 初始化：选取首、尾、中间的中位数
//     int mid = p + (r - p) / 2;
//     int pivot = ((A[p] <= A[mid] && A[mid] <= A[r]) || (A[r] <= A[mid] && A[mid] <= A[p])) ? mid : (((A[mid] <= A[p] && A[p] <= A[r]) || (A[r] <= A[p] && A[p] <= A[mid])) ? p : r);
//     std::swap(A[pivot],A[r]); // 把我选定的基准交换到尾部，这样就无需修改其他的分区逻辑

//     // Partition逻辑主体
//     T x = A[r];
//     int i = p-1;
//     for(int j = p; j < r; j++){
//         if (A[j]<x){
//             i++;
//             std::swap(A[i],A[j]);
//         }
//     }
//     std::swap(A[i+1],A[r]);
//     return i+1;
// }

// template <typename T>
// void quickSort(std::vector<T> &A, int p = 0, int r = -1){
//     if(r == -1)r=A.size()-1;
//     if(p < r){
//         int q = Partition(A, p, r);
//         quickSort(A, p, q-1);
//         quickSort(A, q+1, r);
//     }
//     return;
// }

#pragma once
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream> // 增加了这个头文件用于打印

template <typename T>
int Partition(std::vector<T> &A, int p, int r){
    // 诊断信息：打印进入Partition时的状态
    std::cout << "  [Partition] p=" << p << ", r=" << r << std::endl;

    // 初始化：选取首、尾、中间的中位数
    int mid = p + (r - p) / 2;

    // 诊断信息：在访问数组前，打印将要访问的索引
    std::cout << "    Accessing A[" << p << "], A[" << mid << "], A[" << r << "]" << std::endl;
    if (p < 0 || mid < 0 || r < 0 || r >= A.size() || mid >= A.size() || p >= A.size()) {
        std::cout << "    FATAL: Index out of bounds before pivot selection!" << std::endl;
        exit(1); // 如果索引非法，直接退出
    }

    int pivot = ((A[p] <= A[mid] && A[mid] <= A[r]) || (A[r] <= A[mid] && A[mid] <= A[p])) ? mid : (((A[mid] <= A[p] && A[p] <= A[r]) || (A[r] <= A[p] && A[p] <= A[mid])) ? p : r);
    std::swap(A[pivot],A[r]); 

    T x = A[r];
    int i = p-1;
    for(int j = p; j < r; j++){
        if (A[j]<x){
            i++;
            std::swap(A[i],A[j]);
        }
    }
    std::swap(A[i+1],A[r]);

    // 诊断信息：打印Partition的返回值
    std::cout << "  [Partition] returning " << i + 1 << std::endl;
    return i+1;
}

template <typename T>
void quickSort(std::vector<T> &A, int p = 0, int r = -1){
    if(r == -1)r=A.size()-1;

    // 诊断信息：打印进入quickSort时的状态
    std::cout << "[quickSort] p=" << p << ", r=" << r << std::endl;

    if(p < r){
        int q = Partition(A, p, r);
        quickSort(A, p, q-1);
        quickSort(A, q+1, r);
    }
    return;
}