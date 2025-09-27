#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <fstream>

// ---------------- 排序函数 ----------------
// 插入排序
template <typename T>
void insertionSort(std::vector<T>& A) {
    for (size_t i = 1; i < A.size(); i++) {
        T key = A[i];
        int j = i - 1;
        while (j >= 0 && A[j] > key) {
            A[j + 1] = A[j];
            j--;
        }
        A[j + 1] = key;
    }
}

// 快速排序（经典版）
template <typename T>
int Partition(std::vector<T>& A, int p, int r) {
    T x = A[r];
    int i = p - 1;
    for (int j = p; j < r; j++) {
        if (A[j] < x) {
            i++;
            std::swap(A[i], A[j]);
        }
    }
    std::swap(A[i + 1], A[r]);
    return i + 1;
}

template <typename T>
void quickSort(std::vector<T>& A, int p, int r) {
    if (p < r) {
        int q = Partition(A, p, r);
        quickSort(A, p, q - 1);
        quickSort(A, q + 1, r);
    }
}

template <typename T>
void quickSort(std::vector<T>& A) {
    if (!A.empty()) quickSort(A, 0, A.size() - 1);
}

// ---------------- 工具函数 ----------------
template <typename T>
void randomizeArray(std::vector<T>& A, int range) {
    for (size_t i = 0; i < A.size(); i++) {
        A[i] = rand() % (range + 1);
    }
}

// 计时函数：多次运行，只在循环外调用一次 now()
template <typename T>
double getSortTime(void (*sort)(std::vector<T>&), 
                   const std::vector<T>& baseArray, 
                   int repeat = 1000) {
    using clock = std::chrono::high_resolution_clock;

    auto start = clock::now();
    for (int i = 0; i < repeat; i++) {
        std::vector<T> data = baseArray;  // 拷贝一份基准数据
        sort(data);

        // 防止编译器优化掉
        volatile T sink = data[0];
        (void)sink;
    }
    auto end = clock::now();

    auto total = std::chrono::duration<double, std::micro>(end - start).count();
    return total / repeat;  // 返回平均耗时（单位：微秒）
}

// ---------------- 主函数 ----------------
int main() {
    srand(42); // 固定随机种子，保证可重复实验
    std::ofstream fout("sort_results.csv");
    fout << "N,insertion_us,quick_us\n";

    const int RANGE = 10000;   // 随机数范围
    const int REPEAT = 1000;   // 每个 N 重复次数

    for (int N = 1; N <= 100; N ++) {
        std::vector<int> baseArray(N);
        randomizeArray(baseArray, RANGE);

        double t_insertion = getSortTime(insertionSort<int>, baseArray, REPEAT);
        double t_quick     = getSortTime(quickSort<int>, baseArray, REPEAT);

        fout << N << "," << t_insertion << "," << t_quick << "\n";
        std::cout << "N=" << N 
                  << " insertion=" << t_insertion 
                  << "us quick=" << t_quick << "us\n";
    }

    fout.close();
    std::cout << "结果已保存到 sort_results.csv\n";
    return 0;
}
