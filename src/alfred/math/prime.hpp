#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#include "../core/types.hpp"

using u64 = uint64_t;
using u128 = __uint128_t;

namespace nt {

inline u64 mul(u64 a, u64 b, u64 m) { return u128(a) * b % m; }

inline u64 pw(u64 a, u64 b, u64 m) {
	u64 r = 1;
	for (; b; b >>= 1, a = mul(a, a, m)) {
		if (b & 1) r = mul(r, a, m);
	}
	return r;
}

inline bool isp(u64 n) {
	if (n < 2) return false;
	for (u64 p :
	     {2ull, 3ull, 5ull, 7ull, 11ull, 13ull, 17ull, 19ull, 23ull, 29ull,
	      31ull, 37ull}) {
		if (n % p == 0) return n == p;
	}
	u64 d = n - 1;
	int s = 0;
	for (; ~d & 1; d >>= 1) s++;
	for (u64 a :
	     {2ull, 325ull, 9375ull, 28178ull, 450775ull, 9780504ull,
	      1795265022ull}) {
		if (a % n == 0) continue;
		u64 x = pw(a % n, d, n);
		if (x == 1 || x == n - 1) continue;
		for (int i = 1; i < s; i++) {
			x = mul(x, x, n);
			if (x == n - 1) break;
		}
		if (x != n - 1) return false;
	}
	return true;
}

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
	if (isp(n)) {
		v.push_back({n, 1});
		return;
	}
	u64 d = rho(n);
	fac(d, v), fac(n / d, v);
}

} // namespace nt

template <class T>
bool is_prime(T n) {
	static_assert(is_integral<T>::value, "integral type required");
	if constexpr (is_signed_int<T>::value) {
		if (n < 0) return false;
	}
	if (n < 2) return false;
	if constexpr (sizeof(T) <= 8) {
		return nt::isp(static_cast<u64>(n));
	}
	for (T d = 2; d <= n / d; d++) {
		if (n % d == 0) return false;
	}
	return true;
}

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
	static_assert(is_integral<T>::value, "integral type required");
	if constexpr (is_signed_int<T>::value) {
		assert(n >= 0);
	}
	std::vector<std::pair<T, int>> res;
	if (n < 2) return res;
	if constexpr (sizeof(T) <= 8) {
		std::vector<std::pair<u64, int>> v;
		nt::fac(static_cast<u64>(n), v);
		std::sort(v.begin(), v.end());
		for (auto [p, e] : v) {
			if (!res.empty() && res.back().first == static_cast<T>(p)) {
				res.back().second += e;
			} else {
				res.push_back({static_cast<T>(p), e});
			}
		}
	} else {
		for (T d = 2; d <= n / d; d++) {
			if (n % d) continue;
			int e = 0;
			for (; n % d == 0; n /= d) e++;
			res.push_back({d, e});
		}
		if (n != 1) res.push_back({n, 1});
	}
	return res;
}

template <class T>
inline T phi(T n) {
	for (auto [p, e] : factorize(n)) {
		n = n / p * (p - 1);
	}
	return n;
}
