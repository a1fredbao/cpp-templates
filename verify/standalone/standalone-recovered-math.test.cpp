// competitive-verifier: STANDALONE

#include "../../src/alfred/math/batch_inv.hpp"
#include "../../src/alfred/math/lagrange.hpp"
#include "../../src/alfred/math/speed_of_light_power.hpp"
#include <cassert>
#include <random>
#include <vector>

using mint = m998;

int main() {
	std::vector<mint> a{1, 2, 3, 0, 5};
	auto inv = batch_inv(a);
	assert(inv[0] == mint(1));
	assert(inv[1] == mint(2).inv());
	assert(inv[2] == mint(3).inv());
	assert(inv[3] == mint(0));
	assert(inv[4] == mint(5).inv());

	std::vector<mint> x{0, 1, 2, 3, 4}, y(5);
	for (int i = 0; i < 5; i++) {
		auto v = x[i];
		y[i] = v * v * v + 2 * v + 5;
	}
	Lagrange<mint> lag(x, y);
	for (int i = 0; i < 20; i++) {
		mint k = i * 7 + 3;
		assert(lag.query(k) == k * k * k + 2 * k + 5);
	}

	std::vector<mint> cy{0};
	mint sum = 0;
	for (int i = 1; i <= 8; i++) {
		sum += mint(i).pow(3);
		cy.push_back(sum);
	}
	for (int i = 0; i <= 20; i++) {
		mint want = 0;
		for (int j = 1; j <= i; j++) want += mint(j).pow(3);
		assert(cont_lagrange(cy, i) == want);
	}
	for (int k = 0; k <= 8; k++) {
		for (int i = 0; i <= 20; i++) {
			mint want = 0;
			for (int j = 1; j <= i; j++) want += mint(j).pow(k);
			assert(sum_of_kth_powers(mint(i), k) == want);
		}
	}

	std::mt19937 rng(12345);
	for (int tc = 0; tc < 200; tc++) {
		int n = 1 + rng() % 8;
		std::vector<mint> px(n), xx(n), yy(n);
		for (auto &x : px) x = rng() % 1000;
		for (int i = 0; i < n; i++) xx[i] = i;
		for (int i = 0; i < n; i++) {
			mint cur = 1;
			for (int j = 0; j < n; j++) {
				yy[i] += px[j] * cur;
				cur *= i;
			}
		}
		Lagrange<mint> lag2(xx, yy);
		auto coef = lag2.coef();
		for (int i = 0; i < 20; i++) {
			mint k = rng() % 2000, want = 0, cur = 1;
			for (int j = 0; j < n; j++) {
				want += coef[j] * cur;
				cur *= k;
			}
			assert(lag2.query(k) == want);
		}
	}

	SOLPower<2, mint> sp;
	for (int i = 0; i < 1000; i++) {
		assert(sp.power(i) == mint(2).pow(i));
	}
	long long big = 998244352ll;
	assert(sp.power(big) == 1);
	return 0;
}
