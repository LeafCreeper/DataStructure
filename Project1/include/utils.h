#include <vector>

// 一个用来打印的辅助函数
template <typename T>
void printVector(const std::vector<T> &A){
    for(int i=0;i<A.size();i++)std::cout << A[i] << " ";
    std::cout << std::endl;
}

// 