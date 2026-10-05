// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sqrt_mod

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/modint.hpp"
#include "../../../src/alfred/math/number_theory.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int T;
	optimizeIO(), std::cin >> T;

	while (T--) {
		long long y, p;
		std::cin >> y >> p;
		if (p == 2) {
			std::cout << (y == 0 ? 0 : 1) << '\n';
			continue;
		}
		DynamicModInt::set_mod(p);
		dint x = y;
		if (y == 0) {
			std::cout << 0 << '\n';
		} else if (x.pow((p - 1) / 2) != 1) {
			std::cout << "-1\n";
		} else {
			std::cout << mod_sqrt(x) << '\n';
		}
	}

	return 0;
}
