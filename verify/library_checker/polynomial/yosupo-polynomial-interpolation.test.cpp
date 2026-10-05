// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_interpolation

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/poly.hpp"
#include <iostream>

int main() {
	int n;
	optimizeIO(), std::cin >> n;
	std::vector<m998> x(n), y(n);
	for (auto &value : x) std::cin >> value;
	for (auto &value : y) std::cin >> value;
	auto result = Poly<m998>::interpolate(x, y);
	for (auto value : result) std::cout << value << ' ';
	return 0;
}
