#pragma once
#include "matrix.h"
#include <cstring>
#include <tuple>
#include <algorithm>

template<typename T>
Matrix<T> matrixMul(const Matrix<T>& A, const Matrix<T>& B){
    assert(A.cols() == B.rows());
    
    Matrix<T> result = Matrix<T>(A.rows(),B.cols());

    for(size_t i = 0; i < result.rows(); i++){
        for(size_t j = 0; j < result.cols(); j++){
            T sum = T();
            for(size_t k = 0; k < A.cols() ; k++){
                sum += A(i,k)*B(k,j);
            }
            result(i,j)=sum;
        }
    }

    return result;
}

inline bool is2Power(size_t n){
    return n>0&&!(n&(n-1));
}

inline size_t getClosest2Pow(size_t n){
    if(is2Power(n)) return n;
    int i = 0;
    while(n>>(i++));
    return 1<<i;
}

template<typename T>
std::tuple<Matrix<T>, Matrix<T>>
matrixPadding(const Matrix<T>& A, const Matrix<T>& B)
{
    // A: m×k, B: k×n
    assert(A.cols() == B.rows());
    const size_t m = A.rows();
    const size_t k = A.cols();      // == B.rows()
    const size_t n = B.cols();

    // 统一到同一个 2^t 方阵边长
    const size_t p = getClosest2Pow(std::max({m, k, n}));

    // 已经是共同方阵大小，直接返回
    if (A.rows() == p && A.cols() == p && B.rows() == p && B.cols() == p) {
        return {A, B};
    }

    // 目标方阵并清零
    Matrix<T> A_pad(p, p);
    Matrix<T> B_pad(p, p);
    A_pad.fill(T{});
    B_pad.fill(T{});

    // 行拷贝：假定 row-major 且 ld()==cols()（若你的 Matrix 允许非连续子视图，请改为按元素拷贝）
    for (size_t i = 0; i < m; ++i) {
        std::memcpy(A_pad.data() + i * A_pad.ld(),
                    A.data()     + i * A.ld(),
                    k * sizeof(T));
    }
    for (size_t i = 0; i < k; ++i) {
        std::memcpy(B_pad.data() + i * B_pad.ld(),
                    B.data()     + i * B.ld(),
                    n * sizeof(T));
    }

    return {std::move(A_pad), std::move(B_pad)};
}


template<typename T>
Matrix<T> matrix4CombineTo1(const Matrix<T>& M1,
                            const Matrix<T>& M2,
                            const Matrix<T>& M3,
                            const Matrix<T>& M4,
                            size_t n)
{
    Matrix<T> result(2 * n, 2 * n);

    for (size_t i = 0; i < n; ++i) {
        // 左半部分 (M1)
        std::memcpy(result.data() + i * result.ld(),
                    M1.data() + i * M1.ld(),
                    n * sizeof(T));

        // 右半部分 (M2)
        std::memcpy(result.data() + i * result.ld() + n,
                    M2.data() + i * M2.ld(),
                    n * sizeof(T));
    }

    for (size_t i = 0; i < n; ++i) {
        // 下半部分起始行是 n+i
        std::memcpy(result.data() + (n + i) * result.ld(),
                    M3.data() + i * M3.ld(),
                    n * sizeof(T));

        std::memcpy(result.data() + (n + i) * result.ld() + n,
                    M4.data() + i * M4.ld(),
                    n * sizeof(T));
    }

    return result;
}


template<typename T>
std::tuple<Matrix<T>, Matrix<T>, Matrix<T>, Matrix<T>>
matrixSplit4(const Matrix<T>& M)
{
    size_t n = M.rows();
    assert(n % 2 == 0 && M.cols() == n);  // Strassen需要方阵且可对半分

    size_t half = n / 2;

    Matrix<T> M11 = M.SubMatrix(0,     0,     half, half);
    Matrix<T> M12 = M.SubMatrix(0,     half,  half, half);
    Matrix<T> M21 = M.SubMatrix(half,  0,     half, half);
    Matrix<T> M22 = M.SubMatrix(half,  half,  half, half);

    return {M11, M12, M21, M22};
}

template<typename T>
Matrix<T> matrixMulStrassen_padded(const Matrix<T>& A, const Matrix<T>& B){
    assert(A.cols() == B.rows());
    assert(A.cols() == A.rows() && B.cols() == B.rows());
    // Strassen算法要求A和B都是n = 2^k的方阵，所以要做一个padding。这个函数是默认已经完成padding的
    size_t n = A.cols();
    if(n<=32){
        return matrixMul(A, B);
    }

    auto [A11, A12, A21, A22] = matrixSplit4(A);
    auto [B11, B12, B21, B22] = matrixSplit4(B);

    Matrix<T> M1, M2, M3, M4, M5, M6, M7;
    M1 = matrixMulStrassen_padded(A11 + A22, B11 + B22);
    M2 = matrixMulStrassen_padded(A21 + A22, B11);
    M3 = matrixMulStrassen_padded(A11, B12 - B22);
    M4 = matrixMulStrassen_padded(A22, B21 - B11);
    M5 = matrixMulStrassen_padded(A11 + A12, B22);
    M6 = matrixMulStrassen_padded(A21 - A11, B11 + B12);
    M7 = matrixMulStrassen_padded(A12 - A22, B21 + B22);

    Matrix<T> C11, C12, C21, C22;
    C11 = M1 + M4 - M5 + M7;
    C12 = M3 + M5;
    C21 = M2 + M4;
    C22 = M1 - M2 + M3 + M6;

    return matrix4CombineTo1(C11, C12, C21, C22, n/2);
}

template <typename T>
Matrix<T> matrixMulStrassen(const Matrix<T>& A, const Matrix<T>& B){
    assert(A.cols()==B.rows());
    const size_t m = A.rows();
    const size_t n = B.cols();

    auto [A_pad, B_pad] = matrixPadding(A, B);
    Matrix<T> C_pad = matrixMulStrassen_padded(A_pad, B_pad);

    // Unpadding 回原始 m×n
    Matrix<T> C(m, n);
    for (size_t i = 0; i < m; ++i) {
        std::memcpy(C.data() + i * C.ld(),
                    C_pad.data() + i * C_pad.ld(),
                    n * sizeof(T));
    }
    return C;
}
