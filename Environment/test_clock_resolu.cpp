#include <iostream>
#include <chrono>
using namespace std;

int main() {
    auto t1 = chrono::high_resolution_clock::now();
    auto t2 = chrono::high_resolution_clock::now();
    long long min_delta = LLONG_MAX;
    for (int i = 0; i < 1000000; i++) {
        t2 = chrono::high_resolution_clock::now();
        auto delta = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        if (delta > 0 && delta < min_delta) {
            min_delta = delta;
        }
        t1 = t2;
    }
    cout << "Smallest measurable delta: " << min_delta << " ns\n";
}
