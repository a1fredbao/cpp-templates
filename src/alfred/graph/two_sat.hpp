#pragma once

#include <utility>
#include <vector>

struct TwoSAT {
	const int n;
	std::vector<std::vector<int>> G, RG;
	std::vector<int> bel;

	explicit TwoSAT(int _n) : n(_n), G(2 * _n), RG(2 * _n) {}

	// Add constraint: res[x] = a => res[y] = b.
	inline void conduct(int x, bool a, int y, bool b) {
		int from = x << 1 | a;
		int to = y << 1 | b;
		G[from].push_back(to);
		RG[to].push_back(from);
	}

	// Add constraint: res[x] = a or res[y] = b.
	inline void add(int x, bool a, int y, bool b) {
		conduct(x, !a, y, b);
		conduct(y, !b, x, a);
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

	inline void init(void) {
		int vertices = 2 * n;
		std::vector<char> visited(vertices, 0);
		std::vector<int> order;
		order.reserve(vertices);
		for (int start = 0; start < vertices; start++) {
			if (visited[start]) continue;
			std::vector<std::pair<int, int>> stack;
			stack.push_back({start, 0});
			visited[start] = 1;
			while (!stack.empty()) {
				auto &[u, index] = stack.back();
				if (index < int(G[u].size())) {
					int v = G[u][index++];
					if (!visited[v]) {
						visited[v] = 1;
						stack.push_back({v, 0});
					}
				} else {
					order.push_back(u);
					stack.pop_back();
				}
			}
		}
		bel.assign(vertices, -1);
		int component = 0;
		for (int i = vertices - 1; i >= 0; i--) {
			int start = order[i];
			if (bel[start] != -1) continue;
			std::vector<int> stack = {start};
			bel[start] = component;
			while (!stack.empty()) {
				int u = stack.back();
				stack.pop_back();
				for (int v : RG[u]) {
					if (bel[v] == -1) {
						bel[v] = component;
						stack.push_back(v);
					}
				}
			}
			component++;
		}
	}

	inline bool has_solution(void) {
		for (int i = 0; i < n; i++) {
			if (bel[i << 1] == bel[i << 1 | 1]) return false;
		}
		return true;
	}

	std::vector<int> solve(void) {
		std::vector<int> result(n);
		for (int i = 0; i < n; i++) {
			result[i] = bel[i << 1] < bel[i << 1 | 1];
		}
		return result;
	}
};
