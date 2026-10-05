// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/factorize

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/prime.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int q;
	optimizeIO(), std::cin >> q;

	while (q--) {
		unsigned long long a;
		std::cin >> a;
		auto f = factorize(a);
		int k = 0;
		for (auto &[p, e] : f) k += e;
		std::cout << k;
		for (auto &[p, e] : f) {
			for (int i = 0; i < e; i++) std::cout << ' ' << p;
		}
		std::cout << '\n';
	}

	return 0;
}
