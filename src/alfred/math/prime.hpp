#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

namespace nt {

using u64 = uint64_t;
using u128 = __uint128_t;

inline u64 mul(u64 a, u64 b, u64 m) { return u128(a) * b % m; }

inline u64 pw(u64 a, u64 b, u64 m) {
	u64 r = 1;
	for (; b; b >>= 1, a = mul(a, a, m)) {
		if (b & 1) r = mul(r, a, m);
	}
	return r;
}

} // namespace nt

template <class T>
inline bool is_prime(T n) {
	static_assert(
	    std::is_integral_v<T> && sizeof(T) <= 8,
	    "is_prime supports integral types up to 64 bits"
	);
	if constexpr (std::is_signed_v<T>) {
		if (n < 0) return false;
	}

	uint64_t x = n;
	if (x < 2) return false;
	if (~x & 1) return x == 2;
	uint64_t d = x - 1;
	int s = __builtin_ctzll(d);
	d >>= s;
	for (uint64_t a :
	     {2ull, 325ull, 9375ull, 28178ull, 450775ull, 9780504ull,
	      1795265022ull}) {
		if (a % x == 0) continue;
		uint64_t y = nt::pw(a % x, d, x);
		if (y == 1 || y == x - 1) continue;
		for (int i = 1; i < s && y != x - 1; i++) {
			y = nt::mul(y, y, x);
		}
		if (y != x - 1) return false;
	}
	return true;
}

namespace nt {

inline u64 rho(u64 n) {
	if (~n & 1) return 2;
	if (n % 3 == 0) return 3;
	for (u64 c = 1;; c++) {
		auto f = [&](u64 x) { return (mul(x, x, n) + c) % n; };
		u64 x = 2, y = 2, g = 1;
		while (g == 1) {
			x = f(x), y = f(f(y));
			g = std::gcd(x > y ? x - y : y - x, n);
		}
		if (g != n) return g;
	}
}

inline void fac(u64 n, std::vector<std::pair<u64, int>> &v) {
	if (n == 1) return;
	if (::is_prime(n)) {
		v.push_back({n, 1});
		return;
	}
	u64 d = rho(n);
	fac(d, v), fac(n / d, v);
}

} // namespace nt

inline std::pair<std::vector<int>, std::vector<int>> euler_sieve(int n) {
	std::vector<int> p, lp(n + 1);
	for (int i = 2; i <= n; i++) {
		if (!lp[i]) lp[i] = i, p.push_back(i);
		for (int q : p) {
			if (i * q > n) break;
			lp[i * q] = q;
			if (i % q == 0) break;
		}
	}
	return {p, lp};
}

template <class T>
std::vector<std::pair<T, int>> factorize(T n) {
	static_assert(
	    std::is_integral_v<T> && sizeof(T) <= 8,
	    "factorize supports integral types up to 64 bits"
	);
	if constexpr (std::is_signed_v<T>) {
		assert(n >= 0);
	}
	std::vector<std::pair<T, int>> res;
	if (n < 2) return res;
	std::vector<std::pair<nt::u64, int>> v;
	nt::fac(static_cast<nt::u64>(n), v);
	std::sort(v.begin(), v.end());
	for (auto &[p, e] : v) {
		if (!res.empty() && res.back().first == static_cast<T>(p)) {
			res.back().second += e;
		} else {
			res.push_back({static_cast<T>(p), e});
		}
	}
	return res;
}

template <class T>
inline T phi(T n) {
	for (auto [p, e] : factorize(n)) n = n / p * (p - 1);
	return n;
}
