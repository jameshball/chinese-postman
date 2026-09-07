#pragma once

#include <list>
#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <unordered_map>
using namespace std;

class Graph
{
public:
	//n is the number of vertices
	//edges is a list of pairs representing the edges (default = empty list)
	Graph(int n, list< pair<int, int> > & edges);
    explicit Graph(int n);

	//Default constructor creates an empty graph
	Graph(): n(0), m(0) {};

	//Returns the number of vertices
	int GetNumVertices() { return n; };
	//Returns the number of edges
	int GetNumEdges() { return m; };

	//Given the edge's index, returns its endpoints as a pair
	pair<int, int> GetEdge(int e);
	//Given the endpoints, returns the index
	int GetEdgeIndex(int u, int v);


	//Returns the adjacency list of a vertex
	const vector<int>& AdjList(int v);

	//Returns the graph's adjacency matrix
	bool IsAdjacent(int u, int v);
    void OrderAdjacency(const vector<int>& visitOrder);
private:
    void AddEdge(int u, int v);
	//Number of vertices
	int n;
	//Number of edges
	int m;

	//Adjacency matrix
	bool complete = false;
    vector<int> completeRowStart;

	//Adjacency lists
	vector<vector<int> > adjList;

	//Array of edges
	vector<pair<int, int> > edges;

	//Indices of the edges
	unordered_map<uint64_t, int> edgeIndex;
    uint64_t key(int u, int v) const { return uint64_t(std::min(u, v)) * uint64_t(n) + uint64_t(std::max(u, v)); }
};
