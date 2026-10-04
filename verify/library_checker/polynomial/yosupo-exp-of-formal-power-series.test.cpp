// competitive-verifier: PROBLEM
// https://judge.yosupo.jp/problem/exp_of_formal_power_series

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/poly.hpp"
#include <iostream>

int main() {
	int n;
	optimizeIO(), std::cin >> n;
	Poly<m998> a(n);
	for (auto &x : a) std::cin >> x;
	auto result = a.exp(n);
	for (auto x : result) std::cout << x << ' ';
	return 0;
}
