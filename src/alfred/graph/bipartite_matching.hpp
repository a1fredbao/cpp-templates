#pragma once

#include "dinic.hpp"
#include <cassert>
#include <utility>
#include <vector>

class BipartiteMatching {
public:
	explicit BipartiteMatching(int left, int right)
	    : L(left), R(right), t(left + right + 1),
	      flow(left + right + 2, left + right) {
		for (int u = 0; u < L; u++) {
			flow.add(0, u + 1, 1);
		}
		for (int v = 0; v < R; v++) {
			flow.add(L + v + 1, t, 1);
		}
	}
	inline void add_edge(int u, int v) { flow.add(u + 1, L + v + 1, 1); }
	inline int max_matching(void) {
		if (!solved) {
			siz = flow.maxflow(0, t);
			solved = true;
		}
		return siz;
	}
	std::vector<std::pair<int, int>> matching(void) {
		max_matching();
		std::vector<std::pair<int, int>> res;
		for (auto &edge : flow.edges()) {
			bool ll = 1 <= edge.from && edge.from <= L;
			bool rr = L + 1 <= edge.to && edge.to <= L + R;
			if (ll && rr && edge.flow == 1) {
				res.push_back({edge.from - 1, edge.to - L - 1});
			}
		}
		return res;
	}

private:
	int L, R, t, siz = 0;
	bool solved = false;
	MaxFlow<int> flow;
};
