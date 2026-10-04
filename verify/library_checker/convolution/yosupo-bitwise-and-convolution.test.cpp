// competitive-verifier: PROBLEM
// https://judge.yosupo.jp/problem/bitwise_and_convolution

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/fast_walsh_hadamard.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n;
	optimizeIO(), std::cin >> n;

	std::vector<m998> f(1 << n), g(1 << n);
	for (auto &x : f) std::cin >> x;
	for (auto &x : g) std::cin >> x;

	auto res = and_conv(f, g);
	for (auto &x : res) std::cout << x << ' ';

	return 0;
}
