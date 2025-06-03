#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>
#include <limits>

using namespace std;

vector<vector<int>> read_tree(const string& filename, int n) {
    vector<vector<int>> adj(n);
    ifstream file(filename);
    int u, v;
    double w;

    while (file >> u >> v >> w) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    return adj;
}

void build_rooted_tree(const vector<vector<int>>& adj, vector<vector<int>>& tree, int u, int parent) {
    for (int v : adj[u]) {
        if (v != parent) {
            tree[u].push_back(v);
            build_rooted_tree(adj, tree, v, u);
        }
    }
}

int compute_min_rounds(const vector<vector<int>>& tree, int u) {
    vector<int> child_rounds;

    for (int v : tree[u]) {
        child_rounds.push_back(compute_min_rounds(tree, v));
    }

    if (child_rounds.empty()) return 0;

    sort(child_rounds.rbegin(), child_rounds.rend());
    int max_rounds = 0;

    for (int i = 0; i < (int)child_rounds.size(); ++i) {
        max_rounds = max(max_rounds, child_rounds[i] + i + 1);
    }

    return max_rounds;
}

void analyze_tree(const string& filename, int n, int repetitions) {
    vector<vector<int>> adj = read_tree(filename, n);
    mt19937 rng(time(0));
    uniform_int_distribution<int> dist(0, n - 1);

    int min_rounds = numeric_limits<int>::max();
    int max_rounds = 0;
    double sum_rounds = 0;

    for (int i = 0; i < repetitions; ++i) {
        int root = dist(rng);
        vector<vector<int>> tree(n);
        build_rooted_tree(adj, tree, root, -1);
        int rounds = compute_min_rounds(tree, root);
        sum_rounds += rounds;
        min_rounds = min(min_rounds, rounds);
        max_rounds = max(max_rounds, rounds);
    }

    cout << "File: " << filename << endl;
    cout << "Repetitions: " << repetitions << endl;
    cout << "Avg rounds: " << sum_rounds / repetitions << endl;
    cout << "Min rounds: " << min_rounds << endl;
    cout << "Max rounds: " << max_rounds << endl;
}

int main() {
    string filename = "prim_n300.txt"; // change as needed
    int n = 300;                       // number of vertices
    int repetitions = 20;

    analyze_tree(filename, n, repetitions);
    return 0;
}