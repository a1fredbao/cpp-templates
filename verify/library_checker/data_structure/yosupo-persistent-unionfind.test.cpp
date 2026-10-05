// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/rollback_dsu.hpp"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[]) {
	int n, q;
	optimizeIO(), std::cin >> n >> q;

	// Version `q` stands for the empty graph $G_{-1}$.
	std::vector<int> type(q), u(q), v(q);
	std::vector<std::vector<int>> children(q + 1);
	for (int i = 0; i < q; i++) {
		int k;
		std::cin >> type[i] >> k >> u[i] >> v[i];
		children[k == -1 ? q : k].push_back(i);
	}

	// Answer every query by walking the version tree with rollbacks.
	std::vector<int> ans(q);
	CancelDSU dsu(n);
	struct Frame {
		int node, idx, merges;
	};
	std::vector<Frame> stk = {
	    {q, 0, 0}
	};
	int merges = 0;
	while (!stk.empty()) {
		Frame &f = stk.back();
		if (f.idx < int(children[f.node].size())) {
			int child = children[f.node][f.idx++];
			int base = merges;
			if (type[child] == 0) {
				dsu.merge(u[child], v[child]), merges++;
			} else {
				ans[child] = dsu.same(u[child], v[child]);
			}
			stk.push_back({child, 0, base});
		} else {
			dsu.cancel(merges - f.merges);
			merges = f.merges;
			stk.pop_back();
		}
	}

	for (int i = 0; i < q; i++) {
		if (type[i] == 1) std::cout << ans[i] << '\n';
	}

	return 0;
}
