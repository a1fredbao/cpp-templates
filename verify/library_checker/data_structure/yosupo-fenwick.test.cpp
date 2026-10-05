// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_range_sum

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/fenwick.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n, q, opt, x, v;
	optimizeIO(), std::cin >> n >> q;

	Fenwick<long long> C(n);
	for (int i = 0; i < n; i++) {
		std::cin >> x, C.update(i, x);
	}

	while (q--) {
		std::cin >> opt >> x >> v;
		if (opt == 0) {
			C.update(x, v);
		} else {
			std::cout << C.query(x, v - 1) << '\n';
		}
	}

	return 0;
}
