// main.cpp
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <cstdlib>   // rand, srand
#include <ctime>

#include "matrix.h"            // Matrix<T> 的定义
#include "matrix_multiplication.h"   

// === 工具：计时器 ===
template <typename F>
double time_ms(F&& f) {
    auto t0 = std::chrono::high_resolution_clock::now();
    f();
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dt = t1 - t0;
    return dt.count();
}

// === 工具：随机填充矩阵 ===

template <typename T>
void fill_random(Matrix<T>& M, T low = T(-1), T high = T(1), unsigned seed = 42) {
    std::srand(seed); // 或者用 std::time(nullptr) 生成不同随机数
    for (size_t i = 0; i < M.rows(); ++i) {
        for (size_t j = 0; j < M.cols(); ++j) {
            double r = static_cast<double>(std::rand()) / RAND_MAX; // [0,1)
            M(i, j) = low + (high - low) * T(r);
        }
    }
}


// === 工具：结果校验（返回最大绝对误差） ===
template <typename T>
double max_abs_diff(const Matrix<T>& A, const Matrix<T>& B) {
    assert(A.rows() == B.rows() && A.cols() == B.cols());
    double maxd = 0.0;
    for (size_t i = 0; i < A.rows(); ++i)
        for (size_t j = 0; j < A.cols(); ++j) {
            double d = std::abs(double(A(i, j)) - double(B(i, j)));
            if (d > maxd) maxd = d;
        }
    return maxd;
}

// === 单次实验：随机矩阵，比较两种算法 ===
template <typename T>
void run_once(size_t m, size_t k, size_t n, unsigned seed = 42, double tol = 1e-8) {
    using std::cout;
    using std::setw;

    Matrix<T> A(m, k), B(k, n);
    fill_random(A, T(-1), T(1), seed);
    fill_random(B, T(-1), T(1), seed + 1);

    Matrix<T> C_naive(m, n), C_strassen(m, n);

    double t_naive = time_ms([&](){
        C_naive = matrixMul(A, B);
    });

    double t_strassen = time_ms([&](){
        C_strassen = matrixMulStrassen(A, B);  // 你外层封装的：内部会 padding + 递归 + unpadding
    });

    double err = max_abs_diff(C_naive, C_strassen);
    bool ok = err <= tol;

    cout << "Size: " << m << "x" << k << " * " << k << "x" << n
         << "  ==>  naive: " << std::fixed << std::setprecision(3) << t_naive << " ms"
         << " | strassen: " << t_strassen << " ms" << "\n";
}

// === 多组规模对比 ===
int main(int argc, char** argv) {
    using T = double;

    std::cout << "Matrix Multiply Test (naive vs Strassen)\n";
    std::cout << "----------------------------------------\n";
    struct Case { size_t m, k, n; };
    std::vector<Case> cases = {
        {32, 32, 32}, 
        {64, 64, 64},  
        {128, 128, 128},     
        {256, 256, 256},  
        {512, 512, 512},   
        {1024, 1024, 1024}, 
        {2024, 2048, 2048}
    };

    for (auto c : cases) {
        run_once<T>(c.m, c.k, c.n, 12345);
    }

    return 0;
}
