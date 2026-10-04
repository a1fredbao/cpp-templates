// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/dsu.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n, q, opt, u, v;
	optimizeIO(), std::cin >> n >> q;

	DSU D(n);
	while (q--) {
		std::cin >> opt >> u >> v;
		if (opt == 0) {
			D.merge(u, v);
		} else {
			std::cout << D.same(u, v) << '\n';
		}
	}

	return 0;
}
