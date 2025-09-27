#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
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

double testSortTime(void (*sort)(vector<int>&), int length, int repeat){
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

int main(){
    ofstream fout("result1.csv");
    fout << "N,insert_ms,quick_ms" << endl;

    int maxLength = 100;
    int repeat = 100000;

    int last_n_quicksort_slower = 0;
    for(int n = 1;n<maxLength;n++){
        double insert_ms = testSortTime(insertionSort<int>, n, repeat);
        double quick_ms = testSortTime(quickSort<int>, n, repeat);

        if(quick_ms > insert_ms)last_n_quicksort_slower = n;

        fout << n << "," << insert_ms << "," << quick_ms << endl;
        cout << n << "," << insert_ms << "," << quick_ms << endl;
    }

    fout.close();

    cout << "quicksort gets faster than insertionsort since n = " << last_n_quicksort_slower << endl;


    return 0;

}