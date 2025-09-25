// src/main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <fstream>
#include <cmath>

#include "../include/utils.h"
#include "insertionsort.h"
#include "mergesort.h"
#include "quicksort.h"

#define RANGE 10000

// 包装不同接口的排序函数，方便测试时以统一接口调用
auto merge_wrapper = [](std::vector<int>& v){ mergeSort(v, 0, v.size()-1); };
auto quick_wrapper = [](std::vector<int>& v){ quickSort(v, 0, v.size()-1); };
auto insertion_wrapper = [](std::vector<int>& v){ insertionSort(v); };
std::function<void(std::vector<int>&)> merge_func = merge_wrapper;
std::function<void(std::vector<int>&)> insertion_func = insertion_wrapper;
std::function<void(std::vector<int>&)> quick_func = quick_wrapper;


// 统一的测试接口
template <typename T>
double getSortTime(std::function<void(std::vector<T>&)> sort, std::vector<T> dataArray){
    auto start = std::chrono::high_resolution_clock::now();
    sort(dataArray);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    return duration.count();
}

// 对数均匀采样
std::vector<size_t> logspace(size_t startN, size_t endN, int numSamples) {
    std::vector<size_t> samples;
    double startLog = std::log10(startN);
    double endLog = std::log10(endN);
    double stepLog = (endLog - startLog)/(numSamples-1);
    int sample;
    for(int i=0;i<numSamples;i++){
        double val = std::pow(10, startLog + i * stepLog);
        samples.push_back(static_cast<size_t>(std::round(val)));
    }
    return samples;
}

int main() {
    srand(time(NULL));
    std::vector<size_t> samples = logspace(10, 5000, 100);
    // 在N>5000时出现Segment fault，可能是quick sort带来stack overflow导致的
    std::ofstream csv("results.csv");
    csv << "N,merge_sort_ms,insertion_sort_ms,quick_sort_ms\n";

    for(size_t N:samples){
        std::vector<int> testArray(N);
        std::cout << "N = " << N << " started" << std::endl;
        randomizeArray(testArray, N, RANGE);
        printVector(testArray);
        auto mergeSort_ms = getSortTime<int>(merge_func, testArray);
        std::cout << "N = " << N << " mergesort succeeded" << std::endl;
        auto insertionSort_ms = getSortTime<int>(insertion_func, testArray);
        std::cout << "N = " << N << " insertionsort succeeded" << std::endl;
        auto quickSort_ms = getSortTime<int>(quick_func, testArray);
        std::cout << "N = " << N << " quicksort succeeded" << std::endl;

        csv << N << "," << mergeSort_ms << "," << insertionSort_ms << "," << quickSort_ms << std::endl;
        std::cout << "N = " << N << " is completed" << std::endl;
    }
    csv.close();
    std::cout << "finished" << std::endl;

    return 0;
}
