// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/primality_test

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/prime.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int q;
	optimizeIO(), std::cin >> q;

	while (q--) {
		unsigned long long a;
		std::cin >> a;
		if (is_prime(a)) {
			std::cout << "Yes\n";
		} else {
			std::cout << "No\n";
		}
	}

	return 0;
}
