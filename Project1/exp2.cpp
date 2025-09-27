#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <functional>
#define RANGE 32767

using namespace std;
using clock_my = chrono::high_resolution_clock;

template <typename T>
void insertionSort(vector<T>& A){
    int n = A.size();
    for(int i = 1; i < n; i++){
        T key = A[i];
        int j = i-1;
        while(j>=0 && A[j]>key){
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
    return;
}

template <typename T>
void insertionSort(vector<T>& A, int p, int r){
    for(int i = p + 1; i <= r; i++){
        T key = A[i];
        int j = i-1;
        while(j>=p && A[j]>key){
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
    return;
}

template <typename T>
int partition(vector<T>& A, int p, int r){
    T pivot = A[r];
    int q = p - 1;
    for(int i = p; i < r; i++){
        if(A[i] <= pivot){
            q++;
            swap(A[i],A[q]);
        }

    }
    swap(A[q+1],A[r]);
    return q+1;
}

template <typename T>
void quickSort(vector<T>& A, int p, int r){
    if(r <= p)return;
    int q = partition(A, p, r);
    quickSort(A, p, q-1);
    quickSort(A, q+1 ,r);
    return;
}

template <typename T>
void quickSort(vector<T>& A){
    quickSort(A, 0, A.size()-1);
    return;
}

template <typename F>
double testSortTime(F sort, int length, int repeat){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, RANGE - 1);

    auto start = clock_my::now();
    for(int i = 0; i < repeat; i++){
        vector<int> v(length);
        for(int j = 0; j < length; j++){
            v[j] = distrib(gen);
        }

        sort(v);
    }
    auto end = clock_my::now();

    auto total = std::chrono::duration<double, std::micro>(end - start).count();
    return total / repeat;  // 返回在大量不同随机数组上的平均耗时
}

template <typename T>
void hybridSort(vector<T>& A, int p, int r, int k){
    if(r <= p)return;
    if(r - p < k){
        insertionSort(A, p, r);
        return;
    }
    int q = partition(A, p, r);
    hybridSort(A, p, q-1,k);
    hybridSort(A, q+1 ,r,k);
    return;
}

// 封装 hybridSort(k) 作为一个 sort 函数指针
function<void(vector<int>&)> makeHybridSort(int k) {
    return [k](vector<int>& A){
        hybridSort(A, 0, A.size()-1, k);
    };
}

int main(){
    int length = 1000;
    int repeat = 1000;
    int mink = 5;
    int maxk = 100;

    ofstream fout("result2.csv");
    fout << "k,N,hybrid_ms" << endl;

    int best_time = 10000;
    int best_k;

    for(int k = mink; k <= maxk; k++){
        auto hybrid_k = makeHybridSort(k);
        double time_k = testSortTime(hybrid_k, length, repeat);

        if(time_k < best_time){
            best_time = time_k;
            best_k = k;
        }

        fout << k << "," << length << "," << time_k << endl;
        cout << k << "," << length << "," << time_k << endl;
    }

    fout.close();
    cout << "best k is "<< best_k << ", with best time is " << best_time;

    return 0;
}