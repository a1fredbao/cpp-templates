// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/polynomial/ntt.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n, m;
	optimizeIO(), std::cin >> n >> m;

	std::vector<m998> f(n), g(m);
	for (auto &x : f) std::cin >> x;
	for (auto &x : g) std::cin >> x;

	auto res = add_conv(f, g);
	for (auto &x : res) std::cout << x << ' ';

	return 0;
}
