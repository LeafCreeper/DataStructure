#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Job {
    int t, d, p;
};

bool cmp(const Job &a, const Job &b) {
    return a.d < b.d;
}

int maxProfitScheduling(vector<Job> jobs) {
    int N = jobs.size();

    // 1. 按 deadline 排序
    sort(jobs.begin(), jobs.end(), cmp);

    // 2. 计算开始时间 start[i] = d - t
    vector<int> start(N);
    for (int i = 0; i < N; i++) {
        start[i] = jobs[i].d - jobs[i].t;
    }

    // 3. 计算 pred[i]
    // pred[i] = 最后一个满足 jobs[j].d <= start[i] 的 j
    vector<int> pred(N, -1);

    for (int i = 0; i < N; i++) {
        int lo = 0, hi = i - 1, ans = -1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (jobs[mid].d <= start[i]) {
                ans = mid;         // mid 可作为候选
                lo = mid + 1;      // 往右找更大的
            } else {
                hi = mid - 1;
            }
        }
        pred[i] = ans;
    }

    // 4. dp[i] = 在前 i 个任务中可获得最大收益
    vector<int> dp(N, 0);

    dp[0] = (jobs[0].d >= jobs[0].t ? jobs[0].p : 0);

    for (int i = 1; i < N; i++) {
        int skip = dp[i-1];

        int take = jobs[i].p;
        if (pred[i] != -1) take += dp[pred[i]];

        dp[i] = max(skip, take);
    }

    return dp[N-1];
}


int main() {

    cout << "===== USE CASE 1 =====\n";
    {
        vector<Job> jobs = {
            {2, 3, 40},
            {1, 1, 20},
            {2, 5, 60},
            {1, 2, 30}
        };
        cout << "Max profit = " << maxProfitScheduling(jobs) << "\n\n";
    }

    cout << "===== USE CASE 2 =====\n";
    {
        vector<Job> jobs = {
            {3, 4, 50},  // 必须在 0-4 内连续3小时
            {1, 2, 30},
            {1, 3, 25},
            {2, 3, 40}
        };
        cout << "Max profit = " << maxProfitScheduling(jobs) << "\n\n";
    }

    cout << "===== USE CASE 3 =====\n";
    {
        // 大 deadline + 小 deadline 混合，测试 pred 和 DP 的正确性
        vector<Job> jobs = {
            {3, 100, 100}, // A
            {1, 2, 60},    // B
            {1, 2, 60},    // C
            {2, 3, 90}     // D
        };
        cout << "Max profit = " << maxProfitScheduling(jobs) << "\n\n";
    }

    return 0;
}