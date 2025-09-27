// include/utils.h
#include <vector>
#include <cstdlib>
#include <ctime>

// 一个用来打印的辅助函数
template <typename T>
void printVector(const std::vector<T> &A){
    for(size_t i=0;i<A.size();i++)std::cout << A[i] << " ";
    std::cout << std::endl;
}

// 一个用来生成随机数组的函数
template <typename T>
void randomizeArray(std::vector<T> &A, size_t N, int range){
    for(size_t i = 0; i < N; i++){
        A[i] = rand()%(range + 1);
    }
}