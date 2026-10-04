// competitive-verifier: PROBLEM
// https://judge.yosupo.jp/problem/unionfind_with_potential

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/weighted_dsu.hpp"
#include "../../../src/alfred/math/modint.hpp"
#include <iostream>

int n, q, opt, u, v, x;
using mint = ModInt<998244353>;

int main(int argc, char const *argv[]) {
	optimizeIO(), std::cin >> n >> q;

	WeightedDSU<mint> dsu(n);

	while (q--) {
		std::cin >> opt >> u >> v;
		if (opt == 0) {
			std::cin >> x;
			std::cout << dsu.merge(u, v, x) << '\n';
		} else if (!dsu.same(u, v)) {
			std::cout << "-1\n";
		} else {
			std::cout << dsu.distance(u, v) << '\n';
		}
	}

	return 0;
}
