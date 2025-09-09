#include <vector>

// 实现泛型的插入排序
template <typename T>
void insertionSort(std::vector<T> &A){
    int n = A.size();
    for(int j=1;j<n;j++){
        T key = A[j];
        // 把A[j]插入到已经完成排序的A[1 .. j-1]中
        int i = j-1;
        while(i>=0 && A[i]>key){
            A[i+1] = A[i]; // 把A[j-1]及以前的依次后移，直到找到该插入的位置为止。如果到了i=0还插不上，就说明应该插在第一位
            i = i-1;
        }
        A[i+1] = key;
    }
}

