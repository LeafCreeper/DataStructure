#include "map.h"
#include <limits>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <unordered_set>

using namespace std;


static const double INF = numeric_limits<double>::infinity();

// 线性查边权（小图够用）
double edge_weight(const graph& G, int u, int v) {
    for (auto& e : G.neighbors(u)) if (e.to == v) return e.w;
    return INF;
}

double path_cost(const graph& G, const vector<int>& path) {
    double sum = 0.0;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        double w = edge_weight(G, path[i], path[i+1]);
        sum += w;
    }
    return sum;
}

// 单源dijkstra算法
// 根据预先设定好的容器“接”返回值，后面两个参数都是用来"to restore"的
void dijkstra(const graph& G, int s, vector<double>& dist, vector<int>& parent){
    const int WHITE = 0;
    const int BLACK = 1;
    const int n = G.vsize();
    vector<int> color(n, WHITE);
    // vector<int> parent(n, -1);
    // vector<double> dist(n, INF);
    // 这里写错了，既然目的是让外面的容器收到节点，那就不应该在内层重新声明

    dist.assign(n, INF);
    parent.assign(n, -1);

    dist[s] = 0.0;

    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> Q;
    Q.push({dist[s], s});

    while(!Q.empty()){
        auto [d,v] = Q.top(); Q.pop();
        if (d > dist[v]) continue; // 这一步是为了适应decrease_key的等价实现
        color[v] = BLACK;

        for(auto& e : G.neighbors(v)){
            int u = e.to;
            double w = e.w;
            if(color[u] != BLACK){ // 这个条件判断实际上是不需要的，因为即使不保留它，BLACK的节点本来也就不可能再被松弛。
                                    // 而且跳过这些节点对性能的优化可以忽略不计
                if(dist[u] > dist[v] + w){
                    dist[u] = dist[v] + w;
                    parent[u] = v;
                    Q.push({dist[u], u});
                    // 这里其实应该是decrease_key操作，但是标准库的priority_queue没有提供这个操作
                    // GPT老师说，这里的替代策略是，用重复的Q.push(d, u) 这实际上意味着优先队列中
                    // 会存在多个重复的条目，有新的有旧的，新的的d一定比旧的小，而dist[]中存储的是最小的那个
                    // 所以一种实现策略是，在每次top+pop的时候，检查d和dist的大小关系。如果d > dist，那就意味着这是一个旧条目，忽略不管
                    // 但是我的策略是，直接标记节点的颜色，取出去之后标记为黑色，这样后续即使重复遇到，也一定不需要松弛。这种实现也是没问题的
                }
            }
        }
    }
}

// 实现dijkstra遍历
void all_dijkstra(const graph& G, vector<vector<double>>& dist, vector<vector<int>>& parent){
    const int n = G.vsize();
    dist.resize(n);
    parent.resize(n);
    for(int i = 0; i < n; i++){
        dijkstra(G, i, dist[i], parent[i]);
    }
}

// 从 parent 矩阵恢复 s->t 路径
vector<int> restore_path(int s, int t, const vector<vector<int>>& parent) {
    vector<int> path;
    int cur = t;
    while (cur != -1) {
        path.push_back(cur);
        if (cur == s) break;
        cur = parent[s][cur];
    }
    if (path.empty() || path.back() != s) { path.clear(); return path; }
    reverse(path.begin(), path.end());
    return path;
}

// 输出 routing detail
void print_route(const graph& G, const vector<int>& path) {
    cout << "Path: ";
    for (int i = 0; i < (int)path.size(); i++) {
        if (i) cout << " -> ";
        cout << G.name(path[i]);
    }
    cout << "\n";

    cout << "Routing detail:\n";
    double sum = 0.0;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int u = path[i], v = path[i+1];
        double w = edge_weight(G, u, v);
        sum += w;
        cout << "  " << G.name(u) << " -> " << G.name(v)
             << " (" << fixed << setprecision(2) << w << "), cumulative = "
             << fixed << setprecision(2) << sum << "\n";
    }
}

// 用于 Yen：带“禁用节点/禁用边”的 dijkstra，返回 spur_path（从 s 到 t）
vector<int> dijkstra_banned(const graph& G, int s, int t,
                            const vector<char>& banned_node,
                            const unordered_set<long long>& banned_edge) {
    int n = G.vsize();
    vector<double> dist(n, INF);
    vector<int> parent(n, -1);

    if (banned_node[s] || banned_node[t]) return {};

    using P = pair<double, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    dist[s] = 0.0;
    pq.push({0.0, s});

    auto pack = [](int u, int v) -> long long {
        return ( (long long)u << 32 ) ^ (unsigned int)v;
    };

    while (!pq.empty()) {
        auto [d, v] = pq.top(); pq.pop();
        if (d > dist[v]) continue;
        if (v == t) break;

        for (auto& e : G.neighbors(v)) {
            int u = e.to;
            if (banned_node[u]) continue;
            if (banned_edge.find(pack(v, u)) != banned_edge.end()) continue;

            double w = e.w;
            if (dist[v] + w < dist[u]) {
                dist[u] = dist[v] + w;
                parent[u] = v;
                pq.push({dist[u], u});
            }
        }
    }

    if (!isfinite(dist[t])) return {};

    vector<int> path;
    for (int cur = t; cur != -1; cur = parent[cur]) {
        path.push_back(cur);
        if (cur == s) break;
    }
    if (path.back() != s) return {};
    reverse(path.begin(), path.end());
    return path;
}

// Yen's K-shortest loopless paths（K=3）
vector<vector<int>> yen_top_k(const graph& G, int s, int t, int K,
                             const vector<vector<int>>& parent_apsp,
                             const vector<vector<double>>& dist_apsp) {
    vector<vector<int>> A; // 已确定的最短路径序列
    vector<vector<int>> Bpaths;
    vector<double> Bcosts;

    // 最短路来自你已有的 APSP parent
    if (!isfinite(dist_apsp[s][t])) return {};
    vector<int> P0 = restore_path(s, t, parent_apsp);
    if (P0.empty()) return {};
    A.push_back(P0);

    auto pack = [](int u, int v) -> long long {
        return ( (long long)u << 32 ) ^ (unsigned int)v;
    };

    auto path_key = [](const vector<int>& p) {
        string k;
        for (int x : p) { k += to_string(x); k += ","; }
        return k;
    };
    unordered_set<string> inB, inA;
    inA.insert(path_key(P0));

    // 用一个“手写候选池”（小图+K=3 足够）
    for (int k = 1; k < K; k++) {
        const vector<int>& prev = A.back();

        // 针对 prev 的每个 spur node 生成候选
        for (int i = 0; i + 1 < (int)prev.size(); i++) {
            int spur = prev[i];
            vector<int> root(prev.begin(), prev.begin() + i + 1);

            // banned nodes：root 中除 spur 外全部禁用，防止成环
            vector<char> banned_node(G.vsize(), 0);
            for (int r = 0; r < (int)root.size() - 1; r++) banned_node[root[r]] = 1;

            // banned edges：对所有已确定路径 A，凡是和 root 前缀相同的，禁用下一条边
            unordered_set<long long> banned_edge;
            for (auto& p : A) {
                if ((int)p.size() > i && equal(root.begin(), root.end(), p.begin())) {
                    banned_edge.insert(pack(p[i], p[i+1]));
                }
            }

            // 计算 spur path
            vector<int> spur_path = dijkstra_banned(G, spur, t, banned_node, banned_edge);
            if (spur_path.empty()) continue;

            // 拼接 total path = root + spur_path(去掉第一个 spur)
            vector<int> total = root;
            total.insert(total.end(), spur_path.begin() + 1, spur_path.end());

            string key = path_key(total);
            if (inA.count(key) || inB.count(key)) continue;

            double cost = path_cost(G, total);
            inB.insert(key);
            Bpaths.push_back(total);
            Bcosts.push_back(cost);
        }

        // 从 B 里选最小 cost 的作为下一条
        if (Bpaths.empty()) break;

        int best = 0;
        for (int i = 1; i < (int)Bpaths.size(); i++) {
            if (Bcosts[i] < Bcosts[best]) best = i;
        }

        A.push_back(Bpaths[best]);
        inA.insert(path_key(Bpaths[best]));

        // 从候选池移除该元素
        Bpaths.erase(Bpaths.begin() + best);
        Bcosts.erase(Bcosts.begin() + best);
    }

    return A;
}

static void print_one(const graph& G,
                      const vector<int>& path,
                      double cost,
                      double best_cost,
                      const string& tag)
{
    int hops = path.empty() ? 0 : (int)path.size() - 1;
    double detour = (best_cost > 0 && isfinite(best_cost)) ? (cost / best_cost) : INF;

    cout << fixed << setprecision(2);
    cout << cost
         << "  detour=" << detour
         << "  hops=" << hops
         << "  ";

    for (int i = 0; i < (int)path.size(); i++) {
        if (i) cout << "->";
        cout << G.name(path[i]);
    }
    cout << " (" << tag << ")\n";
}

static bool same_path(const vector<int>& a, const vector<int>& b) {
    return a.size() == b.size() && equal(a.begin(), a.end(), b.begin());
}


vector<int> bfs_least_hop_path(const graph& G, int s, int t) {
    int n = G.vsize();
    vector<int> parent(n, -1);
    vector<char> vis(n, 0);
    queue<int> q;

    vis[s] = 1;
    q.push(s);

    while (!q.empty()) {
        int v = q.front(); q.pop();
        if (v == t) break;

        for (auto& e : G.neighbors(v)) {
            int u = e.to;
            if (!vis[u]) {
                vis[u] = 1;
                parent[u] = v;
                q.push(u);
            }
        }
    }

    if (!vis[t]) return {}; // 不可达

    vector<int> path;
    for (int cur = t; cur != -1; cur = parent[cur]) {
        path.push_back(cur);
        if (cur == s) break;
    }
    reverse(path.begin(), path.end());
    return path;
}



int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    graph G;

    // ===== 1. 读取地图 =====
    string filename = "edge.txt";
    ifstream fin(filename);
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        string a, b;
        double w;
        stringstream ss(line);
        ss >> a >> b >> w;
        if (!ss) continue;

        G.addEdge(a, b, w);
    }
    fin.close();

    int n = G.vsize();
    cout << "Map loaded. Nodes = " << n << endl;

    // ===== 2. APSP 预处理（一次）=====
    vector<vector<double>> dist;
    vector<vector<int>> parent;
    all_dijkstra(G, dist, parent);

    cout << "APSP preprocessing finished." << endl;

        // ===== 3. 循环查询 =====
    cout << "Enter queries (start end), Ctrl+D to exit:" << endl;

    string startName, endName;
    while (cin >> startName >> endName) {
        int s = G.idOf(startName);
        int t = G.idOf(endName);

        if (s == -1 || t == -1) {
            cout << "Unknown node name. start=" << startName
                << " end=" << endName << endl << endl;
            continue;
        }
        if (!isfinite(dist[s][t])) {
            cout << "No path from " << startName << " to " << endName << "." << endl << endl;
            continue;
        }

        // ===== 1) shortest-distance 路径（来自 APSP parent）=====
        vector<int> shortest = restore_path(s, t, parent);
        double best_cost = path_cost(G, shortest);

        // ===== 2) least-hop 路径（BFS）=====
        vector<int> leastHop = bfs_least_hop_path(G, s, t);

        // ===== 3) Yen 候选=====
        vector<vector<int>> yen = yen_top_k(G, s, t, 5, parent, dist);
        vector<pair<vector<int>, string>> chosen;

        // 先放 shortest
        chosen.push_back({shortest, "shortest time"});

        // 再放 least-hop（如果不同）
        if (!leastHop.empty() && !same_path(leastHop, shortest)) {
            chosen.push_back({leastHop, "least hop"});
        }

        // 其余用 Yen 补齐，标签 alternative；跳过与已选重复的路径
        for (auto& p : yen) {
            if ((int)chosen.size() >= 3) break;
            bool dup = false;
            for (auto& c : chosen) {
                if (same_path(p, c.first)) { dup = true; break; }
            }
            if (!dup) chosen.push_back({p, "alternative"});
        }

        // ===== 5) 输出 =====
        cout << "3 routes " << startName << " -> " << endName
            << "  (best=" << fixed << setprecision(2) << best_cost << ")"  << endl;

        for (int i = 0; i < (int)chosen.size(); i++) {
            double c = path_cost(G, chosen[i].first);
            cout << "#" << (i + 1) << " ";
            print_one(G, chosen[i].first, c, best_cost, chosen[i].second);
        }
        cout << endl;
    }

    cout << endl << "Bye.";
    return 0;
}
