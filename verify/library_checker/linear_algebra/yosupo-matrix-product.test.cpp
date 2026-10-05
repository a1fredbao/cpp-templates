// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_product

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/math/linear_algebra.hpp"
#include "../../../src/alfred/math/modint.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int n, m, k;
	optimizeIO(), std::cin >> n >> m >> k;

	Matrix<m998> a(n, m), b(m, k);
	for (auto &row : a) {
		for (auto &x : row) std::cin >> x;
	}
	for (auto &row : b) {
		for (auto &x : row) std::cin >> x;
	}

	auto c = a * b;
	for (auto &row : c) {
		for (auto &x : row) std::cout << x << ' ';
		std::cout << '\n';
	}

	return 0;
}
