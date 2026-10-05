// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_floor_of_linear

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/floor_sum.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int T;
	optimizeIO(), std::cin >> T;
	while (T--) {
		int n, m, a, b;
		std::cin >> n >> m >> a >> b;
		std::cout << floor_sum(a, b, m, n) << '\n';
	}
	return 0;
}
