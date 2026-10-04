// competitive-verifier: PROBLEM
// https://judge.yosupo.jp/problem/multipoint_evaluation

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/poly.hpp"
#include <iostream>

int main() {
	int n, m;
	optimizeIO(), std::cin >> n >> m;
	Poly<m998> f(n);
	std::vector<m998> x(m);
	for (auto &value : f) std::cin >> value;
	for (auto &value : x) std::cin >> value;
	auto result = f.eval(x);
	for (auto value : result) std::cout << value << ' ';
	return 0;
}
