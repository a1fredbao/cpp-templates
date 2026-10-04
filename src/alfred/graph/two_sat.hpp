#ifndef AFGR_2SAT
#define AFGR_2SAT

#include <vector>

struct TwoSAT {
	const int n;
	std::vector<std::vector<int>> G;
	std::vector<int> dfn, low, stk, bel, in;
	TwoSAT(int _n) : n(_n), G(2 * _n) {} // 0-indexed, n nodes.
	void tarjan(int u, int &cnt, int &cc) {
		dfn[u] = low[u] = ++cnt;
		in[u] = 1, stk.push_back(u);
		for (auto &v : G[u]) {
			if (dfn[v] == 0) {
				tarjan(v, cnt, cc);
				low[u] = std::min(low[u], low[v]);
			} else if (in[v]) {
				low[u] = std::min(low[u], dfn[v]);
			}
		}
		int v;
		if (dfn[u] == low[u]) {
			cc++;
			do {
				v = stk.back(), in[v] = 0;
				stk.pop_back(), bel[v] = cc;
			} while (v != u);
		}
	}
	// Add constraint: res[x] = a => res[y] = b.
	inline void conduct(int x, bool a, int y, bool b) {
		G[x << 1 | a].push_back(y << 1 | b);
	}
	// Add constraint: res[x] = a or res[y] = b.
	inline void add(int x, bool a, int y, bool b) {
		conduct(x, !a, y, b), conduct(y, !b, x, a);
	}
	// Add constraint: res[x] = res[y].
	inline void same(int x, int y) {
		conduct(x, true, y, true);
		conduct(y, true, x, true);
		conduct(x, false, y, false);
		conduct(y, false, x, false);
	}
	// Add constraint: res[x] != res[y].
	inline void diff(int x, int y) {
		conduct(x, true, y, false);
		conduct(y, true, x, false);
		conduct(x, false, y, true);
		conduct(y, false, x, true);
	}
	// Add constraint: res[x] = v.
	inline void set(int x, bool v) { conduct(x, !v, x, v); }
	inline void init(void) { // find scc.
		int n = G.size(), cnt = 0, cc = 0;
		in = dfn = low = bel = std::vector<int>(n, 0);
		for (int i = 0; i < n; i++) {
			if (!dfn[i]) tarjan(i, cnt, cc);
		}
	}
	inline bool has_solution(void) {
		int var = G.size() / 2;
		for (int i = 0; i < var; i++) {
			if (bel[i << 1] == bel[i << 1 | 1]) {
				return false;
			}
		}
		return true;
	}
	std::vector<int> solve(void) {
		int var = G.size() / 2;
		std::vector<int> sol(n);
		for (int i = 0; i < var; i++) {
			sol[i] = bel[i << 1] > bel[i << 1 | 1];
		}
		return sol;
	}
};

#endif
