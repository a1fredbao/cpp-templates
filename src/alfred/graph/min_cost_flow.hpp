#ifndef AFGR_MIN_COST_MAXFLOW
#define AFGR_MIN_COST_MAXFLOW

#include <algorithm>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

template <class Cap, class Cost>
struct MCMF_SPFA {
	struct _Edge {
		int to;
		Cap cap;
		Cost cost;
		_Edge(int v, Cap c, Cost w) : to(v), cap(c), cost(w) {}
	};
	int n;
	std::vector<_Edge> e;
	std::vector<Cost> dis;
	std::vector<int> pre, pre_edge;
	std::vector<bool> in_queue;
	std::vector<std::vector<int>> G;
	MCMF_SPFA(void) {}
	MCMF_SPFA(int n, int m = 0) { init(n, m); }
	inline void init(int n, int m = 0) {
		this->n = ++n;
		G.assign(n, {});
		dis.resize(n);
		pre.resize(n);
		pre_edge.resize(n);
		in_queue.resize(n);
		e.reserve(2 * m);
	}
	inline void add(int u, int v, Cap c, Cost w) {
		G[u].push_back(e.size());
		e.emplace_back(v, c, w);
		G[v].push_back(e.size());
		e.emplace_back(u, 0, -w);
	}
	inline bool spfa(int s, int t) {
		std::queue<int> Q;
		std::fill(dis.begin(), dis.end(), std::numeric_limits<Cost>::max());
		std::fill(in_queue.begin(), in_queue.end(), false);
		dis[s] = 0, in_queue[s] = true, Q.push(s);
		while (!Q.empty()) {
			int u = Q.front();
			Q.pop(), in_queue[u] = false;
			for (auto &i : G[u]) {
				auto &[v, c, w] = e[i];
				if (c > 0 && dis[v] > dis[u] + w) {
					dis[v] = dis[u] + w;
					pre[v] = u;
					pre_edge[v] = i;
					if (!in_queue[v]) {
						in_queue[v] = true, Q.push(v);
					}
				}
			}
		}
		return dis[t] != std::numeric_limits<Cost>::max();
	}
	inline std::pair<Cap, Cost> maxflow(int s, int t) {
		Cap flow = 0;
		Cost cost = 0;
		while (spfa(s, t)) {
			Cap f = std::numeric_limits<Cap>::max();
			for (int u = t; u != s; u = pre[u]) {
				f = std::min(f, e[pre_edge[u]].cap);
			}
			for (int u = t; u != s; u = pre[u]) {
				e[pre_edge[u]].cap -= f;
				e[pre_edge[u] ^ 1].cap += f;
				cost += f * e[pre_edge[u]].cost;
			}
			flow += f;
		}
		return {flow, cost};
	}

	struct Edge {
		int from, to;
		Cap cap, flow;
		Cost cost;
	};
	inline std::vector<Edge> edges(void) {
		std::vector<Edge> a;
		for (size_t i = 0; i < e.size(); i += 2) {
			a.push_back(
			    {e[i + 1].to, e[i].to, e[i].cap + e[i + 1].cap, e[i + 1].cap,
				 e[i].cost}
			);
		}
		return a;
	}
};

template <class Cap, class Cost>
struct MCMF_Dijkstra {
	struct _Edge {
		int to;
		Cap cap;
		Cost cost;
		_Edge(int v, Cap c, Cost w) : to(v), cap(c), cost(w) {}
	};
	int n;
	std::vector<_Edge> e;
	std::vector<Cost> dis, pot;
	std::vector<int> pre, pre_edge;
	std::vector<bool> vis;
	std::vector<std::vector<int>> G;
	MCMF_Dijkstra(void) {}
	MCMF_Dijkstra(int n, int m = 0) { init(n, m); }
	inline void init(int n, int m = 0) {
		this->n = ++n;
		G.assign(n, {});
		dis.resize(n);
		pot.assign(n, 0);
		pre.resize(n);
		pre_edge.resize(n);
		vis.resize(n);
		e.reserve(2 * m);
	}
	inline void add(int u, int v, Cap c, Cost w) {
		G[u].push_back(e.size());
		e.emplace_back(v, c, w);
		G[v].push_back(e.size());
		e.emplace_back(u, 0, -w);
	}
	inline bool dijkstra(int s, int t) {
		using P = std::pair<Cost, int>;
		std::priority_queue<P, std::vector<P>, std::greater<P>> Q;
		std::fill(dis.begin(), dis.end(), std::numeric_limits<Cost>::max());
		std::fill(vis.begin(), vis.end(), false);
		dis[s] = 0, Q.emplace(0, s);
		while (!Q.empty()) {
			auto [d, u] = Q.top();
			Q.pop();
			if (vis[u]) continue;
			vis[u] = true;
			for (auto &i : G[u]) {
				auto &[v, c, w] = e[i];
				if (c > 0 && dis[v] > dis[u] + w + pot[u] - pot[v]) {
					dis[v] = dis[u] + w + pot[u] - pot[v];
					pre[v] = u;
					pre_edge[v] = i;
					Q.emplace(dis[v], v);
				}
			}
		}
		return dis[t] != std::numeric_limits<Cost>::max();
	}
	inline std::pair<Cap, Cost> maxflow(int s, int t) {
		Cap flow = 0;
		Cost cost = 0;
		while (dijkstra(s, t)) {
			for (int i = 0; i < n; ++i) {
				if (dis[i] != std::numeric_limits<Cost>::max()) {
					pot[i] += dis[i];
				}
			}
			Cap f = std::numeric_limits<Cap>::max();
			for (int u = t; u != s; u = pre[u]) {
				f = std::min(f, e[pre_edge[u]].cap);
			}
			for (int u = t; u != s; u = pre[u]) {
				e[pre_edge[u]].cap -= f;
				e[pre_edge[u] ^ 1].cap += f;
				cost += f * e[pre_edge[u]].cost;
			}
			flow += f;
		}
		return {flow, cost};
	}

	struct Edge {
		int from, to;
		Cap cap, flow;
		Cost cost;
	};
	inline std::vector<Edge> edges(void) {
		std::vector<Edge> a;
		for (size_t i = 0; i < e.size(); i += 2) {
			a.push_back(
			    {e[i + 1].to, e[i].to, e[i].cap + e[i + 1].cap, e[i + 1].cap,
				 e[i].cost}
			);
		}
		return a;
	}
};

#endif
