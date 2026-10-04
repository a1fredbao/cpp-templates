#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "../core/types.hpp"

namespace prime_detail {

using u64 = uint64_t;
using u128 = __uint128_t;

inline u64 mul_mod(u64 a, u64 b, u64 mod) {
	return static_cast<u64>(u128(a) * b % mod);
}

inline u64 pow_mod(u64 base, u64 exponent, u64 mod) {
	u64 result = 1;
	while (exponent != 0) {
		if (exponent & 1) result = mul_mod(result, base, mod);
		base = mul_mod(base, base, mod);
		exponent >>= 1;
	}
	return result;
}

inline bool is_prime_u64(u64 n) {
	if (n < 2) return false;
	for (u64 p :
	     {2ull, 3ull, 5ull, 7ull, 11ull, 13ull, 17ull, 19ull, 23ull, 29ull,
	      31ull, 37ull}) {
		if (n % p == 0) return n == p;
	}
	u64 d = n - 1;
	int shift = 0;
	while ((d & 1) == 0) {
		d >>= 1;
		shift++;
	}
	for (u64 base :
	     {2ull, 325ull, 9375ull, 28178ull, 450775ull, 9780504ull,
	      1795265022ull}) {
		if (base % n == 0) continue;
		u64 value = pow_mod(base % n, d, n);
		if (value == 1 || value == n - 1) continue;
		bool witness = true;
		for (int i = 1; i < shift; i++) {
			value = mul_mod(value, value, n);
			if (value == n - 1) {
				witness = false;
				break;
			}
		}
		if (witness) return false;
	}
	return true;
}

inline u64 pollard_rho(u64 n) {
	if (n % 2 == 0) return 2;
	if (n % 3 == 0) return 3;
	for (u64 constant = 1;; constant++) {
		u64 x = 2, y = 2, divisor = 1;
		auto next = [&](u64 value) {
			return (mul_mod(value, value, n) + constant) % n;
		};
		while (divisor == 1) {
			x = next(x);
			y = next(next(y));
			u64 difference = x > y ? x - y : y - x;
			divisor = std::gcd(difference, n);
		}
		if (divisor != n) return divisor;
	}
}

inline void factorize_u64(u64 n, std::vector<std::pair<u64, int>> &factors) {
	if (n == 1) return;
	if (is_prime_u64(n)) {
		factors.push_back({n, 1});
		return;
	}
	u64 divisor = pollard_rho(n);
	factorize_u64(divisor, factors);
	factorize_u64(n / divisor, factors);
}

} // namespace prime_detail

template <class T>
bool is_prime(T n) {
	static_assert(is_integral<T>::value, "is_prime requires an integral type");
	if constexpr (is_signed_int<T>::value) {
		if (n < 0) return false;
	}
	if (n < 2) return false;
	if constexpr (sizeof(T) <= 8) {
		return prime_detail::is_prime_u64(static_cast<uint64_t>(n));
	}
	for (T divisor = 2; divisor <= n / divisor; divisor++) {
		if (n % divisor == 0) return false;
	}
	return true;
}

// Given in integer n. Returns (primes, minp).
std::pair<std::vector<int>, std::vector<int>> euler_sieve(int n) {
	std::vector<int> primes, minp(n + 1);
	for (int i = 2; i <= n; i++) {
		if (minp[i] == 0) {
			minp[i] = i;
			primes.push_back(i);
		}
		for (auto &p : primes) {
			if (i * p > n) break;
			minp[i * p] = p;
			if (i % p == 0) break;
		}
	}
	return std::make_pair(primes, minp);
}

template <class T>
std::vector<std::pair<T, int>> factorize(T n) {
	static_assert(is_integral<T>::value, "factorize requires an integral type");
	if constexpr (is_signed_int<T>::value) {
		assert(n >= 0);
	}
	std::vector<std::pair<T, int>> result;
	if (n < 2) return result;
	if constexpr (sizeof(T) <= 8) {
		std::vector<std::pair<prime_detail::u64, int>> factors;
		prime_detail::factorize_u64(static_cast<prime_detail::u64>(n), factors);
		std::sort(factors.begin(), factors.end());
		for (auto [prime, exponent] : factors) {
			if (!result.empty() &&
			    result.back().first == static_cast<T>(prime)) {
				result.back().second += exponent;
			} else {
				result.push_back({static_cast<T>(prime), exponent});
			}
		}
	} else {
		for (T divisor = 2; divisor <= n / divisor; divisor++) {
			if (n % divisor != 0) continue;
			int exponent = 0;
			while (n % divisor == 0) {
				n /= divisor;
				exponent++;
			}
			result.push_back({divisor, exponent});
		}
		if (n != 1) result.push_back({n, 1});
	}
	return result;
}

template <class T>
inline T phi(T n) {
	auto factors = factorize(n);
	for (auto [prime, exponent] : factors) {
		n = n / prime * (prime - 1);
	}
	return n;
}
