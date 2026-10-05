// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/binomial_coefficient_prime_mod

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/comb.hpp"
#include "../../../src/alfred/math/modint.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int T, m;
	optimizeIO(), std::cin >> T >> m;

	DynamicModInt::set_mod(m);
	Comb<dint> comb;

	while (T--) {
		int n, k;
		std::cin >> n >> k;
		std::cout << comb.binom(n, k) << '\n';
	}

	return 0;
}
