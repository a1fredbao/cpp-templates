#ifndef AFGR_MAXFLOW
#define AFGR_MAXFLOW

#include <limits>
#include <queue>
#include <vector>

template <class T>
struct MaxFlow {
	struct _Edge {
		int to;
		T cap;
		_Edge(int v, T c) : to(v), cap(c) {}
	};
	int n;
	std::vector<_Edge> e;
	std::vector<int> dep, cur;
	std::vector<std::vector<int>> G;
	MaxFlow(void) {}
	MaxFlow(int n, int m = 0) { init(n, m); }
	inline void init(int n, int m = 0) {
		this->n = ++n, G.assign(n, {});
		dep.resize(n), e.reserve(2 * m);
	}
	inline void add(int u, int v, T c) {
		G[u].push_back(e.size());
		e.emplace_back(v, c);
		G[v].push_back(e.size());
		e.emplace_back(u, 0);
	}
	inline bool bfs(int s, int t) {
		std::queue<int> Q;
		dep.assign(n, -1);
		dep[s] = 0, Q.push(s);
		while (!Q.empty()) {
			s = Q.front(), Q.pop();
			if (s == t) return true;
			for (auto &i : G[s]) {
				auto &[v, c] = e[i];
				if (c > 0 && dep[v] == -1) {
					dep[v] = dep[s] + 1, Q.push(v);
				}
			}
		}
		return false;
	}
	T dfs(int u, int t, T f) {
		if (u == t) {
			return f;
		}
		T r = f;
		for (int &i = cur[u]; i < int(G[u].size()); ++i) {
			const int j = G[u][i];
			auto &[v, c] = e[j];
			if (c > 0 && dep[v] == dep[u] + 1) {
				T a = dfs(v, t, std::min(r, c));
				e[j].cap -= a, e[j ^ 1].cap += a, r -= a;
				if (r == 0) {
					return f;
				}
			}
		}
		return f - r;
	}
	inline T maxflow(int s, int t) {
		T ans = 0;
		while (bfs(s, t)) {
			cur.assign(n, 0);
			ans += dfs(s, t, std::numeric_limits<T>::max());
		}
		return ans;
	}

	struct Edge {
		int from, to;
		T cap, flow;
	};
	inline std::vector<Edge> edges(void) {
		std::vector<Edge> a;
		for (size_t i = 0; i < e.size(); i += 2) {
			a.push_back(
			    {e[i + 1].to, e[i].to, e[i].cap + e[i + 1].cap, e[i + 1].cap}
			);
		}
		return a;
	}
};

#endif
