// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_interpolation

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/poly.hpp"
#include <iostream>

int main() {
	int n;
	optimizeIO(), std::cin >> n;
	std::vector<m998> x(n), y(n);
	for (int i = 0; i < n; i++) std::cin >> x[i] >> y[i];
	auto result = Poly<m998>::interpolate(x, y);
	for (auto value : result) std::cout << value << ' ';
	return 0;
}
