// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/poly.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n, m;
	optimizeIO(), std::cin >> n >> m;

	Poly<m998> f(n), g(m);
	for (auto &x : f) std::cin >> x;
	for (auto &x : g) std::cin >> x;

	auto res = f * g;
	for (auto &x : res) std::cout << x << ' ';

	return 0;
}
