#include <iostream>
#include <vector>
#include <string>

#include "../include/utils.h"
#include "insertionsort.h"
#include "mergesort.h"

int main() {
    // 测试 int 类型
    std::vector<int> nums = {5, 2, 9, 1, 6};

    std::cout << "原始数组 (int): ";
    printVector(nums);

    insertionSort(nums);
    std::cout << "插入排序结果: ";
    printVector(nums);

    // 再测试归并排序
    std::vector<int> nums2 = {10, 3, 7, 8, 2};
    std::cout << "\n原始数组 (int): ";
    printVector(nums2);

    mergeSort(nums2);
    std::cout << "归并排序结果: ";
    printVector(nums2);

    // 测试 string 类型
    std::vector<std::string> words = {"pear", "apple", "banana", "kiwi"};
    std::cout << "\n原始数组 (string): ";
    printVector(words);

    mergeSort(words);
    std::cout << "归并排序结果: ";
    printVector(words);

    return 0;
}
