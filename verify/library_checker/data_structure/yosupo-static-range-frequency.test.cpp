// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_frequency

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/appear_statistics.hpp"
#include <iostream>
#include <vector>

int main() {
	int n, q, l, r, x;
	optimizeIO(), std::cin >> n >> q;
	std::vector<int> a(n);
	for (auto &v : a) std::cin >> v;

	AppearStats<int> as(a);
	while (q--) {
		std::cin >> l >> r >> x;
		std::cout << as.count(l, r - 1, x) << '\n';
	}
	return 0;
}
