#include "Graph.h"
Graph::Graph(int n, list<pair<int, int>>& input) : n(n), m(0), adjList(n) {
    for (auto [u, v] : input) {
        if (u >= 0 && v >= 0 && u < n && v < n) { AddEdge(u, v); }
    }
}
// Complete graphs use the same lexicographic edge order as the postman solver.
Graph::Graph(int n) : n(n), m(0), complete(true), completeRowStart(n), adjList(n) {
    for (int u = 0; u < n; ++u) {
        completeRowStart[u] = m - u - 1;
        for (int v = u + 1; v < n; ++v) {
            edges.emplace_back(u, v);
            adjList[u].push_back(v);
            adjList[v].push_back(u);
            ++m;
        }
    }
}
pair<int, int> Graph::GetEdge(int e) { return edges[e]; }
int Graph::GetEdgeIndex(int u, int v) {
    if (complete) {
        if (u > v) { std::swap(u, v); }
        return completeRowStart[u] + v;
    }
    return edgeIndex.at(key(u, v));
}
void Graph::AddEdge(int u, int v) {
    if (!edgeIndex.emplace(key(u, v), m).second) { return; }
    adjList[u].push_back(v);
    adjList[v].push_back(u);
    edges.emplace_back(u, v);
    ++m;
}
const vector<int>& Graph::AdjList(int v) { return adjList[v]; }
bool Graph::IsAdjacent(int u, int v) {
    return complete ? u != v : edgeIndex.find(key(u, v)) != edgeIndex.end();
}

// Match the edge insertion order of rebuilding this connected component in DFS order.
void Graph::OrderAdjacency(const vector<int>& visitOrder) {
    vector<int> rank(n);
    for (int i = 0; i < n; ++i) { rank[visitOrder[i]] = i; }
    for (int u = 0; u < n; ++u) {
        auto& neighbours = adjList[u];
        auto earlierEnd = std::stable_partition(neighbours.begin(), neighbours.end(), [&](int v) { return rank[v] < rank[u]; });
        std::sort(neighbours.begin(), earlierEnd, [&](int a, int b) { return rank[a] < rank[b]; });
    }
}
