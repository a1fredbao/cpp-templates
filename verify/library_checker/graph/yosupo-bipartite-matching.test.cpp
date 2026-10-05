// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bipartitematching

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/graph/bipartite_matching.hpp"
#include <iostream>

int main() {
	int l, r, m, u, v;
	optimizeIO(), std::cin >> l >> r >> m;

	BipartiteMatching matching(l, r);

	while (m--) {
		std::cin >> u >> v;
		matching.add_edge(u, v);
	}

	std::cout << matching.max_matching() << '\n';
	for (auto [u, v] : matching.matching()) {
		std::cout << u << ' ' << v << '\n';
	}

	return 0;
}
