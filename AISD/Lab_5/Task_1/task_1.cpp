#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

struct Edge {
    int u, v;
    double weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

struct GraphEdge {
    int to;
    double weight;
};

typedef vector<vector<GraphEdge>> AdjList;

// === DSU do Kruskala ===
struct DSU {
    vector<int> parent, rank;

    DSU(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }

    int find(int x) {
        if (x != parent[x])
            parent[x] = find(parent[x]);
        return parent[x];
    }

    bool unite(int x, int y) {
        int xr = find(x), yr = find(y);
        if (xr == yr) return false;
        if (rank[xr] < rank[yr]) parent[xr] = yr;
        else if (rank[xr] > rank[yr]) parent[yr] = xr;
        else { parent[yr] = xr; rank[xr]++; }
        return true;
    }
};

// === Generowanie pełnego grafu ===
void generate_complete_graph(int n, AdjList &adjList, vector<Edge> &edgeList) {
    adjList.assign(n, {});
    edgeList.clear();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double weight = (double)rand() / RAND_MAX;
            adjList[i].push_back({j, weight});
            adjList[j].push_back({i, weight});
            edgeList.push_back({i, j, weight});
        }
    }
}

// === Prim ===
double prim_mst(const AdjList &graph) {
    int n = graph.size();
    vector<bool> visited(n, false);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    double total_weight = 0.0;

    pq.push({0.0, 0});
    while (!pq.empty()) {
        auto [weight, u] = pq.top(); pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        total_weight += weight;

        for (const auto &e : graph[u]) {
            if (!visited[e.to]) {
                pq.push({e.weight, e.to});
            }
        }
    }
    return total_weight;
}

// === Kruskal ===
double kruskal_mst(vector<Edge> edges, int n) {
    sort(edges.begin(), edges.end());
    DSU dsu(n);
    double total_weight = 0.0;
    int edges_used = 0;

    for (const Edge &e : edges) {
        if (dsu.unite(e.u, e.v)) {
            total_weight += e.weight;
            if (++edges_used == n - 1) break;
        }
    }
    return total_weight;
}

int main() {
    srand(time(NULL));

    int nMin = 100;
    int nMax = 2000;
    int step = 100;
    int rep = 5;

    cout << "n\tPrim_time[ms]\tKruskal_time[ms]\n";

    for (int n = nMin; n <= nMax; n += step) {
        double prim_total_time = 0.0;
        double kruskal_total_time = 0.0;

        for (int r = 0; r < rep; ++r) {
            AdjList graph;
            vector<Edge> edges;
            generate_complete_graph(n, graph, edges);

            auto t1 = high_resolution_clock::now();
            double prim_weight = prim_mst(graph);
            auto t2 = high_resolution_clock::now();
            prim_total_time += duration<double, milli>(t2 - t1).count();

            auto t3 = high_resolution_clock::now();
            double kruskal_weight = kruskal_mst(edges, n);
            auto t4 = high_resolution_clock::now();
            kruskal_total_time += duration<double, milli>(t4 - t3).count();
        }

        cout << n << "\t"
             << fixed << setprecision(2)
             << prim_total_time / rep << "\t\t"
             << kruskal_total_time / rep << "\n";
    }

    return 0;
}
