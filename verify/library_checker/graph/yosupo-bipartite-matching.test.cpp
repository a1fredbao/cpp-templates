// competitive-verifier: PROBLEM
// https://judge.yosupo.jp/problem/bipartitematching

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/graph/dinic.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int l, r, m, u, v;
	optimizeIO(), std::cin >> l >> r >> m;

	MaxFlow<int> flow(l + r + 1, m + l + r);

	for (int i = 1; i <= l; i++) flow.add(0, i, 1);
	for (int i = 1; i <= r; i++) {
		flow.add(l + i, l + r + 1, 1);
	}

	while (m--) {
		std::cin >> u >> v;
		flow.add(u + 1, v + l + 1, 1);
	}

	std::cout << flow.maxflow(0, l + r + 1) << '\n';

	for (auto &[u, v, c, f] : flow.edges()) {
		if (u != 0 && v != l + r + 1 && f == 1) {
			std::cout << u - 1 << ' ' << v - l - 1 << '\n';
		}
	}

	return 0;
}
