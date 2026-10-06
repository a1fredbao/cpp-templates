#pragma once

#include "batch_inv.hpp"
#include "comb.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

template <class mint>
class Lagrange {
private:
	std::vector<mint> x, y, b;

public:
	Lagrange(void) = default;
	Lagrange(const std::vector<mint> &x0, const std::vector<mint> &y0) {
		int n = x0.size();
		for (int i = 0; i < n; i++) insert(x0[i], y0[i]);
	}

	inline void insert(mint x0, mint y0) {
		int n = x.size();
		b.push_back(y0);
		std::vector<mint> t(n);
		for (int i = 0; i < n; i++) t[i] = x0 - x[i];
		VecInv<mint> iv(t);
		for (int i = 0; i < n; i++) {
			b.back() *= iv[i], b[i] *= -iv[i];
		}
		x.push_back(x0), y.push_back(y0);
	}

	inline mint query(mint k) {
		int n = x.size();
		std::vector<mint> t(n);
		mint tot = 1;
		for (int i = 0; i < n; i++) {
			if (x[i] == k) return y[i];
			t[i] = k - x[i], tot *= t[i];
		}
		VecInv<mint> iv(t);
		mint ans = 0;
		for (int i = 0; i < n; i++) ans += b[i] * tot * iv[i];
		return ans;
	}

	std::vector<mint> coef(void) {
		int n = x.size();
		std::vector<mint> f(n + 1), ans(n), res(n);
		f[0] = 1;
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j >= 0; j--) {
				f[j] *= -x[i];
				if (j) f[j] += f[j - 1];
			}
		}
		for (int i = 0; i < n; i++) {
			mint d = 0;
			for (int j = n; j > 0; j--) {
				res[j - 1] = f[j] + d;
				d = res[j - 1] * x[i];
			}
			for (int j = 0; j < n; j++) ans[j] += b[i] * res[j];
		}
		return ans;
	}
};

// y[0] is a placeholder; y[i] = f(i) for i in [1, y.size()).
template <class mint, class K>
inline mint cont_lagrange(const std::vector<mint> &y, K k_) {
	int n = y.size() - 1;
	if (n <= 0) return 0;
	mint k = k_;
	Comb<mint> comb(n);
	std::vector<mint> pre(n + 1, 1), suf(n + 2, 1);
	for (int i = 1; i <= n; i++) pre[i] = pre[i - 1] * (k - i);
	for (int i = n; i >= 1; i--) suf[i] = suf[i + 1] * (k - i);
	mint ans = 0;
	for (int i = 1; i <= n; i++) {
		mint a = pre[i - 1] * suf[i + 1];
		mint c = comb.invfac(i - 1) * comb.invfac(n - i);
		ans += ((n - i) & 1 ? -1 : 1) * y[i] * a * c;
	}
	return ans;
}

template <class mint>
inline mint sum_of_kth_powers(mint n, int k) {
	std::vector<mint> y{0};
	mint sum = 0;
	for (int i = 1; i <= k + 2; i++) {
		sum += mint(i).pow(k);
		y.push_back(sum);
	}
	return cont_lagrange(y, n);
}
