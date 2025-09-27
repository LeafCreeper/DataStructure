// src/main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <fstream>
#include <cmath>
#include <random>

#include "../include/utils.h"
#include "insertionsort.h"
#include "mergesort.h"
#include "quicksort.h"

// 统一的测试接口 - 修改后版本
// 不再接收一个固定的基准数组，而是在内部生成新的随机数组
template <typename T>
double getSortTime(void (*sort)(std::vector<T>&),
                   size_t N,
                   int range,
                   int repeat = 1000) {

    using clock = std::chrono::high_resolution_clock;

    // 使用更高质量的 C++11 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, range - 1);

    auto start = clock::now();
    for (int i = 0; i < repeat; i++) {
        // 在每次循环中都创建一个全新的、随机的数组
        std::vector<T> data(N);
        for(size_t j = 0; j < N; ++j) {
            data[j] = distrib(gen);
        }

        sort(data); // 对新生成的随机数组进行排序

        // 防止编译器优化掉。对于N>0的情况总是安全的
        if (N > 0) {
            volatile T sink = data[0];
            (void)sink;
        }
    }
    auto end = clock::now();

    auto total = std::chrono::duration<double, std::micro>(end - start).count();
    return total / repeat;  // 返回在大量不同随机数组上的平均耗时
}

int main() {
    // srand(time(0)); // C-style 随机数生成器，可以被 C++ <random> 替代
    std::ofstream fout("results.csv");
    fout << "N,merge_us,insertion_us,quick_us\n";

    const int RANGE = 10000;  // 随机数范围
    const int REPEAT = 1000;  // 对于每个N，生成并测试1000个不同的随机数组

    for (int N = 1; N <= 100; N++) {
        // 不再需要在这里创建 baseArray 和调用 randomizeArray

        // 直接调用修改后的 getSortTime 函数
        double t_merge = getSortTime(mergeSort<int>, N, RANGE, REPEAT);
        double t_insertion = getSortTime(insertionSort<int>, N, RANGE, REPEAT);
        double t_quick = getSortTime(quickSort<int>, N, RANGE, REPEAT);

        fout << N << "," << t_merge << "," << t_insertion << "," << t_quick << "\n";
        // 建议在控制台输出中也加上 merge_us，保持一致
        std::cout << "N=" << N
                  << " merge=" << t_merge
                  << "us insertion=" << t_insertion
                  << "us quick=" << t_quick << "us\n";
    }

    fout.close();
    std::cout << "结果已保存到 results.csv\n";
    return 0;
}