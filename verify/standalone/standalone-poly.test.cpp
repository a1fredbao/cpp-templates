// competitive-verifier: STANDALONE

#include "../../src/alfred/math/poly.hpp"
#include <cassert>
#include <random>
#include <vector>

using mint = m998;
using PolyType = Poly<mint>;

int main() {
	std::mt19937 rng(12345);

	for (int test = 0; test < 20; test++) {
		int n = 130 + rng() % 100;
		int m = 130 + rng() % 100;
		PolyType a(n), b(m), expected(n + m - 1);
		for (auto &x : a) x = rng() % 100000;
		for (auto &x : b) x = rng() % 100000;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				expected[i + j] += a[i] * b[j];
			}
		}
		assert(a * b == expected);
	}

	for (int test = 0; test < 20; test++) {
		int n = 1 + rng() % 150;
		PolyType a(n);
		for (auto &x : a) x = rng() % 100000;
		a[0] = 1;
		PolyType one(n);
		one[0] = 1;
		assert((a * a.inv(n)).trunc(n) == one);

		PolyType f(n);
		for (int i = 1; i < n; i++) f[i] = rng() % 100000;
		assert(f.exp(n).log(n) == f);
		PolyType root = (a * a).sqrt(n);
		assert((root * root).trunc(n) == (a * a).trunc(n));
	}

	for (int test = 0; test < 20; test++) {
		int n = 1 + rng() % 80;
		PolyType a(n);
		for (auto &x : a) x = rng() % 100000;
		a[0] = 1;
		int exponent = rng() % 7;
		PolyType expected(n);
		expected[0] = 1;
		for (int i = 0; i < exponent; i++) {
			expected = (expected * a).trunc(n);
		}
		assert(a.pow(exponent, n) == expected);
	}

	for (int test = 0; test < 20; test++) {
		int n = 1 + rng() % 80;
		int q = 1 + rng() % 80;
		PolyType f(n);
		std::vector<mint> x(q);
		for (auto &v : f) v = rng() % 100000;
		for (auto &v : x) v = rng() % 100000;
		auto y = f.eval(x);
		for (int i = 0; i < q; i++) {
			mint value = 0;
			for (int j = n - 1; j >= 0; j--) {
				value = value * x[i] + f[j];
			}
			assert(value == y[i]);
		}
	}

	for (int test = 0; test < 10; test++) {
		int n = 1 + rng() % 50;
		std::vector<mint> x(n), y(n);
		for (int i = 0; i < n; i++) x[i] = i;
		for (auto &value : y) value = rng() % 100000;
		assert(PolyType::interpolate(x, y).eval(x) == y);
	}

	PolyType sequence(20);
	sequence[0] = 0;
	sequence[1] = 1;
	for (int i = 2; i < 20; i++) {
		sequence[i] = sequence[i - 1] + sequence[i - 2];
	}
	PolyType recurrence = PolyType::berlekamp_massey(sequence);
	PolyType p{0, 1};
	PolyType q{1, -1, -1};
	for (int i = 0; i < 20; i++) {
		assert(PolyType::linear_recurrence(p, q, i) == sequence[i]);
	}
}
