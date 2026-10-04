#ifndef ALGR_GRAPH
#define ALGR_GRAPH

#include <limits>
#include <numeric>
#include <queue>
#include <vector>

struct null_type {}; // null_type
template <class W = null_type>
class Graph {
private:
	struct Edge {
		int to;
		W w;
	};
	std::vector<std::vector<Edge>> G; // (to, E)

public:
	Graph(void) {}
	Graph(int n) : G(n + 1) {}
	Graph(int n, std::vector<int> _G[]) : G(n + 1) {
		for (int i = 0; i <= n; i++) {
			for (auto &j : _G[i]) { add_directed(i, j); }
		}
	}
	inline void clear(void) { G.clear(); }
	inline size_t size(void) { return G.size(); }
	inline void resize(int n) { G.resize(n + 1); }
	std::vector<Edge> &operator[](int x) { return G[x]; }
	inline void add_directed(int u, int v, W w = W{}) {
		G[u].push_back({v, w});
	}
	inline void add_undirected(int u, int v, W w = W{}) {
		G[u].push_back({v, w});
		G[v].push_back({u, w});
	}
	template <class T = W>
	std::vector<T> dijkstra(int s) { // by default the shortest path.
		using Node = std::pair<T, int>;
		std::vector<T> dis(G.size(), std::numeric_limits<T>::max());
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> heap;
		heap.push({T{}, s});
		while (!heap.empty()) {
			Node t = heap.top();
			heap.pop();
			if (dis[t.second] != t.first) continue;
			for (auto &edge : G[t.second]) {
				if (dis[edge.to] > t.first + edge.w) {
					dis[edge.to] = t.first + edge.w;
					heap.push({dis[edge.to], edge.to});
				}
			}
		}
		return dis;
	}
};

#endif // ALGR_GRAPH
