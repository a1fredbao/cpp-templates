#pragma once

#include "modint.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <utility>
#include <vector>

namespace polynomial_detail {

inline const std::vector<int> &revision(int n) {
	static std::vector<int> rev;
	if (int(rev.size()) != n) {
		rev.resize(n);
		int shift = __builtin_ctz(n) - 1;
		for (int i = 0; i < n; i++) {
			rev[i] = rev[i >> 1] >> 1 | (i & 1) << shift;
		}
	}
	return rev;
}

template <class mint>
const std::vector<mint> &roots(int n) {
	static std::vector<mint> table;
	static uint32_t cached_mod = 0;
	if (cached_mod != mint::mod()) {
		table.clear();
		cached_mod = mint::mod();
	}
	if (int(table.size()) < n) {
		table.assign(n, mint(0));
		table[0] = mint(1);
		for (int len = 2; len <= n; len <<= 1) {
			int half = len >> 1;
			mint w =
			    mint::primitive_root().pow_unsigned((mint::mod() - 1) / len);
			table[half] = mint(1);
			for (int i = 1; i < half; i++) {
				table[half + i] = table[half + i - 1] * w;
			}
		}
	}
	return table;
}

template <class mint>
void dft(std::vector<mint> &a) {
	int n = int(a.size());
	if (n == 1) return;
	const auto &rev = revision(n);
	for (int i = 0; i < n; i++) {
		if (rev[i] < i) std::swap(a[i], a[rev[i]]);
	}
	const auto &root = roots<mint>(n);
	for (int half = 1; half < n; half <<= 1) {
		for (int i = 0; i < n; i += half << 1) {
			for (int j = 0; j < half; j++) {
				mint x = a[i + j];
				mint y = a[i + j + half] * root[half + j];
				a[i + j] = x + y;
				a[i + j + half] = x - y;
			}
		}
	}
}

template <class mint>
void idft(std::vector<mint> &a) {
	int n = int(a.size());
	if (n == 1) return;
	std::reverse(a.begin() + 1, a.end());
	dft(a);
	mint inv = mint(n).inv();
	for (auto &x : a) x *= inv;
}

} // namespace polynomial_detail

template <class mint = m998>
class Poly : public std::vector<mint> {
public:
	using Base = std::vector<mint>;
	using Value = mint;

	Poly() = default;

	explicit Poly(int n) : Base(n) {}

	explicit Poly(const Base &a) : Base(a) {}

	explicit Poly(Base &&a) : Base(std::move(a)) {}

	explicit Poly(std::initializer_list<Value> a) : Base(a) {}

	template <class Iterator>
	explicit Poly(Iterator first, Iterator last) : Base(first, last) {}

	Poly shift(int k) const {
		if (k >= 0) {
			Poly result = *this;
			result.insert(result.begin(), k, Value(0));
			return result;
		}
		if (int(this->size()) <= -k) return Poly();
		return Poly(this->begin() - k, this->end());
	}

	Poly trunc(int k) const {
		assert(k >= 0);
		Poly result = *this;
		if (int(result.size()) > k) result.resize(k);
		return result;
	}

	friend Poly operator+(const Poly &a, const Poly &b) {
		Poly result(std::max(a.size(), b.size()));
		for (int i = 0; i < int(a.size()); i++) result[i] += a[i];
		for (int i = 0; i < int(b.size()); i++) result[i] += b[i];
		return result;
	}

	friend Poly operator-(const Poly &a, const Poly &b) {
		Poly result(std::max(a.size(), b.size()));
		for (int i = 0; i < int(a.size()); i++) result[i] += a[i];
		for (int i = 0; i < int(b.size()); i++) result[i] -= b[i];
		return result;
	}

	friend Poly operator-(const Poly &a) {
		Poly result(a.size());
		for (int i = 0; i < int(a.size()); i++) result[i] = -a[i];
		return result;
	}

	friend Poly operator*(Poly a, Poly b) {
		if (a.empty() || b.empty()) return Poly();
		if (a.size() < b.size()) std::swap(a, b);
		int total = int(a.size() + b.size()) - 1;
		int length = 1;
		while (length < total) length <<= 1;
		if (b.size() < 128 || ((mint::mod() - 1) & (length - 1)) != 0) {
			Poly result(total);
			for (int i = 0; i < int(a.size()); i++) {
				for (int j = 0; j < int(b.size()); j++) {
					result[i + j] += a[i] * b[j];
				}
			}
			return result;
		}
		a.resize(length), b.resize(length);
		polynomial_detail::dft(a);
		polynomial_detail::dft(b);
		for (int i = 0; i < length; i++) a[i] *= b[i];
		polynomial_detail::idft(a);
		a.resize(total);
		return a;
	}

	friend Poly operator*(const Value &a, Poly b) {
		for (auto &x : b) x *= a;
		return b;
	}

	friend Poly operator*(Poly a, const Value &b) {
		for (auto &x : a) x *= b;
		return a;
	}

	friend Poly operator/(Poly a, const Value &b) {
		for (auto &x : a) x /= b;
		return a;
	}

	Poly &operator+=(const Poly &b) { return *this = *this + b; }
	Poly &operator-=(const Poly &b) { return *this = *this - b; }
	Poly &operator*=(const Poly &b) { return *this = *this * b; }
	Poly &operator*=(const Value &b) { return *this = *this * b; }
	Poly &operator/=(const Value &b) { return *this = *this / b; }

	Poly deriv() const {
		if (this->empty()) return Poly();
		Poly result(this->size() - 1);
		for (int i = 0; i < int(this->size()) - 1; i++) {
			result[i] = (*this)[i + 1] * (i + 1);
		}
		return result;
	}

	Poly integr() const {
		Poly result(this->size() + 1);
		for (int i = 0; i < int(this->size()); i++) {
			result[i + 1] = (*this)[i] / (i + 1);
		}
		return result;
	}

	// Requires: the constant coefficient is non-zero.
	Poly inv(int m) const {
		assert(m >= 0);
		assert(!this->empty());
		assert((*this)[0] != 0);
		if (m == 0) return Poly();
		Poly result{(*this)[0].inv()};
		for (int k = 1; k < m; k <<= 1) {
			int length = std::min(k << 1, m);
			result = (result * (Poly{Value(2)} - trunc(length) * result))
			             .trunc(length);
		}
		return result.trunc(m);
	}

	// Requires: the constant coefficient is exactly 1.
	Poly log(int m) const {
		assert(m >= 0);
		assert(!this->empty());
		assert((*this)[0] == 1);
		if (m == 0) return Poly();
		return (deriv() * inv(m)).integr().trunc(m);
	}

	// Requires: the constant coefficient is 0.
	Poly exp(int m) const {
		assert(m >= 0);
		assert(this->empty() || (*this)[0] == 0);
		if (m == 0) return Poly();
		Poly result{Value(1)};
		for (int k = 1; k < m; k <<= 1) {
			int length = std::min(k << 1, m);
			result =
			    (result * (Poly{Value(1)} - result.log(length) + trunc(length)))
			        .trunc(length);
		}
		return result.trunc(m);
	}

	// Requires: exponent >= 0.
	Poly pow(int exponent, int m) const {
		assert(exponent >= 0);
		assert(m >= 0);
		if (m == 0) return Poly();
		int first = 0;
		while (first < int(this->size()) && (*this)[first] == 0) first++;
		if (first == int(this->size()) || 1ll * first * exponent >= m) {
			return Poly(m);
		}
		Value value = (*this)[first];
		Poly base = shift(-first) * value.inv();
		int remainder = m - first * exponent;
		Poly result = (base.log(remainder) * Value(exponent)).exp(remainder);
		return (result.shift(first * exponent) * value.pow(exponent)).trunc(m);
	}

	// Requires: the constant coefficient is exactly 1.
	Poly sqrt(int m) const {
		assert(m >= 0);
		assert(!this->empty());
		assert((*this)[0] == 1);
		if (m == 0) return Poly();
		Poly result{Value(1)};
		for (int k = 1; k < m; k <<= 1) {
			int length = std::min(k << 1, m);
			result =
			    (result + (trunc(length) * result.inv(length)).trunc(length)) *
			    Value(2).inv();
		}
		return result.trunc(m);
	}

	Poly mulT(Poly b) const {
		if (b.empty()) return Poly();
		int n = int(b.size());
		std::reverse(b.begin(), b.end());
		return ((*this) * b).shift(-(n - 1));
	}

	std::vector<Value> eval(std::vector<Value> x) const {
		if (x.empty()) return {};
		int original = int(x.size());
		int n = std::max(original, int(this->size()));
		std::vector<Poly> tree(4 * n);
		std::vector<Value> answer(original);
		x.resize(n);
		std::function<void(int, int, int)> build = [&](int p, int l, int r) {
			if (r - l == 1) {
				tree[p] = Poly{Value(1), -x[l]};
				return;
			}
			int mid = (l + r) >> 1;
			build(p << 1, l, mid);
			build(p << 1 | 1, mid, r);
			tree[p] = tree[p << 1] * tree[p << 1 | 1];
		};
		build(1, 0, n);
		std::function<void(int, int, int, const Poly &)> work =
		    [&](int p, int l, int r, const Poly &num) {
			    if (r - l == 1) {
				    if (l < original) answer[l] = num[0];
				    return;
			    }
			    int mid = (l + r) >> 1;
			    Poly left = num.mulT(tree[p << 1 | 1]);
			    left.resize(mid - l);
			    work(p << 1, l, mid, left);
			    Poly right = num.mulT(tree[p << 1]);
			    right.resize(r - mid);
			    work(p << 1 | 1, mid, r, right);
		    };
		work(1, 0, n, mulT(tree[1].inv(n)));
		return answer;
	}

	static Poly
	interpolate(const std::vector<Value> &x, const std::vector<Value> &y) {
		// Requires: x values are pairwise distinct and lengths match.
		assert(x.size() == y.size());
		int n = int(x.size());
		if (n == 0) return Poly();
		std::vector<Poly> tree(4 * n);
		std::function<void(int, int, int)> build = [&](int p, int l, int r) {
			if (r - l == 1) {
				tree[p] = Poly{-x[l], Value(1)};
				return;
			}
			int mid = (l + r) >> 1;
			build(p << 1, l, mid);
			build(p << 1 | 1, mid, r);
			tree[p] = tree[p << 1] * tree[p << 1 | 1];
		};
		build(1, 0, n);
		std::vector<Value> derivative = tree[1].deriv().eval(x);
		std::vector<Value> weight(n);
		for (int i = 0; i < n; i++) {
			assert(derivative[i] != 0);
			weight[i] = y[i] / derivative[i];
		}
		std::function<Poly(int, int, int)> solve = [&](int p, int l, int r) {
			if (r - l == 1) return Poly{weight[l]};
			int mid = (l + r) >> 1;
			return solve(p << 1, l, mid) * tree[p << 1 | 1] +
			       solve(p << 1 | 1, mid, r) * tree[p << 1];
		};
		return solve(1, 0, n);
	}

	static Poly berlekampMassey(const Poly &sequence) {
		Poly current, previous;
		int last = -1;
		for (int i = 0; i < int(sequence.size()); i++) {
			Value delta = sequence[i];
			for (int j = 1; j <= int(current.size()); j++) {
				delta -= current[j - 1] * sequence[i - j];
			}
			if (delta == 0) continue;
			if (last == -1) {
				current.resize(i + 1);
				last = i;
				continue;
			}
			Poly update = previous;
			update *= Value(-1);
			update.insert(update.begin(), 1);
			Value denominator = 0;
			for (int j = 1; j <= int(update.size()); j++) {
				denominator += update[j - 1] * sequence[last + 1 - j];
			}
			assert(denominator != 0);
			Value coefficient = delta / denominator;
			update *= coefficient;
			Poly shifted(i - last - 1);
			shifted.insert(shifted.end(), update.begin(), update.end());
			Poly old = current;
			current += shifted;
			if (i - int(old.size()) > last - int(previous.size())) {
				previous = old;
				last = i;
			}
		}
		current *= Value(-1);
		current.insert(current.begin(), 1);
		return current;
	}

	static Value linearRecurrence(Poly p, Poly q, int64_t n) {
		int m = int(q.size()) - 1;
		while (n > 0) {
			Poly newq = q;
			for (int i = 1; i <= m; i += 2) newq[i] *= Value(-1);
			Poly newp = p * newq;
			newq = q * newq;
			for (int i = 0; i < m; i++) {
				p[i] = newp[i * 2 + n % 2];
			}
			for (int i = 0; i <= m; i++) {
				q[i] = newq[i * 2];
			}
			n >>= 1;
		}
		return p[0] / q[0];
	}
};
