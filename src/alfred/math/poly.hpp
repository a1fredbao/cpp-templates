#pragma once

#include "modint.hpp"
#include "number_theory.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

template <class mint>
void ntt(std::vector<mint> &a, bool inv) {
	int n = a.size(), s = 0;
	for (; (1 << s) < n; s++);
	static std::vector<mint> ep, iep;
	static uint32_t pm = 0;
	if (pm != mint::mod()) {
		pm = mint::mod(), ep.clear(), iep.clear();
	}
	for (; int(ep.size()) <= s;) {
		ep.push_back(
		    mint::primitive_root().pow((mint::mod() - 1) / (1 << ep.size()))
		);
		iep.push_back(ep.back().inv());
	}
	std::vector<mint> b(n);
	for (int i = 1; i <= s; i++) {
		int w = 1 << (s - i);
		mint base = inv ? iep[i] : ep[i], now = 1;
		for (int y = 0; y < n / 2; y += w) {
			for (int x = 0; x < w; x++) {
				mint l = a[y << 1 | x], r = now * a[y << 1 | x | w];
				b[y | x] = l + r, b[y | x | (n >> 1)] = l - r;
			}
			now *= base;
		}
		std::swap(a, b);
	}
}

template <class mint>
std::vector<mint> mul(std::vector<mint> a, std::vector<mint> b) {
	int n = a.size(), m = b.size();
	if (!n || !m) return {};
	if (std::min(n, m) <= 8) {
		std::vector<mint> c(n + m - 1);
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) c[i + j] += a[i] * b[j];
		}
		return c;
	}
	int z = 1;
	for (; z < n + m - 1; z <<= 1);
	a.resize(z), b.resize(z);
	ntt(a, false), ntt(b, false);
	for (int i = 0; i < z; i++) a[i] *= b[i];
	ntt(a, true), a.resize(n + m - 1);
	mint iz = mint(z).inv();
	for (mint &x : a) x *= iz;
	return a;
}

template <class mint = m998>
struct Poly : std::vector<mint> {
	using V = std::vector<mint>;
	using V::V;

	explicit Poly(const V &a) : V(a) {}
	explicit Poly(V &&a) : V(std::move(a)) {}

	int n() const { return this->size(); }
	mint at(int i) const { return i < n() ? (*this)[i] : mint(0); }

	Poly pre(int k) const {
		int m = std::min(n(), k);
		return Poly(V(this->begin(), this->begin() + m));
	}
	Poly trunc(int k) const { return pre(k); }

	Poly shift(int k) const { // shift right if k > 0, left if k < 0
		Poly r = *this;
		if (k >= 0) r.insert(r.begin(), k, mint(0));
		else r.erase(r.begin(), r.begin() + std::min(-k, n()));
		return r;
	}

	friend Poly operator+(const Poly &a, const Poly &b) {
		Poly r(std::max(a.n(), b.n()));
		for (int i = 0; i < r.n(); i++) r[i] = a.at(i) + b.at(i);
		return r;
	}
	friend Poly operator-(const Poly &a, const Poly &b) {
		Poly r(std::max(a.n(), b.n()));
		for (int i = 0; i < r.n(); i++) r[i] = a.at(i) - b.at(i);
		return r;
	}
	friend Poly operator-(const Poly &a) {
		Poly r = a;
		for (mint &x : r) x = -x;
		return r;
	}
	friend Poly operator*(const Poly &a, const Poly &b) {
		return Poly(mul<mint>(a, b));
	}
	friend Poly operator*(Poly a, const mint &b) {
		for (mint &x : a) x *= b;
		return a;
	}
	friend Poly operator*(const mint &b, Poly a) { return a * b; }
	friend Poly operator/(Poly a, const mint &b) { return a * b.inv(); }

	Poly &operator+=(const Poly &b) { return *this = *this + b; }
	Poly &operator-=(const Poly &b) { return *this = *this - b; }
	Poly &operator*=(const Poly &b) { return *this = *this * b; }
	Poly &operator*=(const mint &b) { return *this = *this * b; }
	Poly &operator/=(const mint &b) { return *this = *this / b; }

	Poly diff() const {
		Poly r(std::max(0, n() - 1));
		for (int i = 1; i < n(); i++) r[i - 1] = at(i) * i;
		return r;
	}
	Poly inte() const {
		Poly r(n() + 1);
		for (int i = 0; i < n(); i++) r[i + 1] = at(i) / (i + 1);
		return r;
	}

	Poly inv(int m) const {
		if (m == 0) return {};
		Poly r{at(0).inv()};
		for (int i = 1; i < m; i *= 2) {
			r = (r * mint(2) - r * r * pre(2 * i)).pre(2 * i);
		}
		return r.pre(m);
	}
	Poly log(int m) const {
		if (m == 0) return {};
		return (pre(m).diff() * pre(m).inv(m - 1)).pre(m - 1).inte();
	}
	Poly exp(int m) const {
		if (m == 0) return {};
		Poly f{1}, g{1};
		for (int i = 1; i < m; i *= 2) {
			g = (g * mint(2) - f * g * g).pre(i);
			Poly q = diff().pre(i - 1);
			Poly w = (q + g * (f.diff() - f * q)).pre(2 * i - 1);
			f = (f + f * (*this - w.inte()).pre(2 * i)).pre(2 * i);
		}
		return f.pre(m);
	}
	Poly pow(int k, int m) const {
		int i = 0;
		while (i < n() && at(i) == 0) i++;
		if (i == n() || 1ll * i * k >= m) return Poly(m);
		mint v = at(i), c = v.pow(k);
		Poly f = shift(-i) * v.inv();
		return ((f.log(m - i * k) * mint(k)).exp(m - i * k).shift(i * k) * c)
		    .pre(m);
	}
	bool has_sqrt() const {
		if (!n()) return true;
		int i = 0;
		for (; i < n() && at(i) == 0; i++);
		if (i == n()) return true;
		if (i & 1) return false;
		return at(i).pow((mint::mod() - 1) / 2) == 1;
	}
	Poly sqrt(int m) const {
		if (m == 0) return {};
		if (!n()) return Poly(m);
		int low = 0;
		for (; low < n() && at(low) == 0; low++);
		if ((low & 1) || low >= m) return Poly(m);
		Poly f(this->begin() + low, this->end());
		Poly g{mod_sqrt(f[0])};
		for (int i = 1; i < m; i *= 2) {
			g = (g + f.pre(2 * i) * g.inv(2 * i)) / mint(2);
		}
		return g.pre(m - low / 2).shift(low / 2);
	}

	Poly mulT(Poly b) const {
		if (!b.n()) return {};
		int m = b.n();
		std::reverse(b.begin(), b.end());
		return ((*this) * b).shift(-(m - 1));
	}

	std::vector<mint> eval(std::vector<mint> x) const {
		if (x.empty()) return {};
		int m = x.size(), sz = std::max(m, n());
		std::vector<Poly> t(4 * sz);
		std::vector<mint> ans(m);
		x.resize(sz);
		std::function<void(int, int, int)> build = [&](int p, int l, int r) {
			if (r - l == 1) t[p] = Poly{1, -x[l]};
			else {
				int mid = (l + r) >> 1;
				build(p << 1, l, mid);
				build(p << 1 | 1, mid, r);
				t[p] = t[p << 1] * t[p << 1 | 1];
			}
		};
		build(1, 0, sz);
		std::function<void(int, int, int, const Poly &)> go =
		    [&](int p, int l, int r, const Poly &f) {
			    if (r - l == 1) {
				    if (l < m) ans[l] = f[0];
				    return;
			    }
			    int mid = (l + r) >> 1;
			    Poly a = f.mulT(t[p << 1 | 1]);
			    a.resize(mid - l);
			    go(p << 1, l, mid, a);
			    Poly b = f.mulT(t[p << 1]);
			    b.resize(r - mid);
			    go(p << 1 | 1, mid, r, b);
		    };
		go(1, 0, sz, mulT(t[1].inv(sz)));
		return ans;
	}

	static Poly
	interpolate(const std::vector<mint> &x, const std::vector<mint> &y) {
		assert(x.size() == y.size());
		int n = x.size();
		if (!n) return {};
		std::vector<Poly> t(4 * n);
		std::function<void(int, int, int)> build = [&](int p, int l, int r) {
			if (r - l == 1) t[p] = Poly{-x[l], 1};
			else {
				int mid = (l + r) >> 1;
				build(p << 1, l, mid);
				build(p << 1 | 1, mid, r);
				t[p] = t[p << 1] * t[p << 1 | 1];
			}
		};
		build(1, 0, n);
		std::vector<mint> d = t[1].diff().eval(x), w(n);
		for (int i = 0; i < n; i++) w[i] = y[i] / d[i];
		std::function<Poly(int, int, int)> solve = [&](int p, int l, int r) {
			if (r - l == 1) return Poly{w[l]};
			int mid = (l + r) >> 1;
			return solve(p << 1, l, mid) * t[p << 1 | 1] +
			       solve(p << 1 | 1, mid, r) * t[p << 1];
		};
		return solve(1, 0, n);
	}

	static Poly berlekamp_massey(const Poly &s) {
		Poly c, old;
		int f = -1;
		for (int i = 0; i < s.n(); i++) {
			mint d = s[i];
			for (int j = 1; j <= c.n(); j++) d -= c[j - 1] * s[i - j];
			if (d == 0) continue;
			if (f == -1) {
				c.resize(i + 1), f = i;
				continue;
			}
			Poly u = old;
			u *= mint(-1), u.insert(u.begin(), 1);
			mint z = 0;
			for (int j = 1; j <= u.n(); j++) z += u[j - 1] * s[f + 1 - j];
			u *= d / z;
			Poly t(i - f - 1);
			t.insert(t.end(), u.begin(), u.end());
			Poly tmp = c;
			c += t;
			if (i - tmp.n() > f - old.n()) old = tmp, f = i;
		}
		c *= mint(-1), c.insert(c.begin(), 1);
		return c;
	}

	static mint linear_recurrence(Poly p, Poly q, int64_t n) {
		int m = q.n() - 1;
		for (; n; n >>= 1) {
			Poly nq = q;
			for (int i = 1; i <= m; i += 2) nq[i] *= mint(-1);
			Poly np = p * nq;
			nq = q * nq;
			for (int i = 0; i < m; i++) p[i] = np[i * 2 + n % 2];
			for (int i = 0; i <= m; i++) q[i] = nq[i * 2];
		}
		return p[0] / q[0];
	}
};
