#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "types.hpp"

template <class T>
inline bool chkmax(T &x, T y) {
	if (x < y) {
		x = y;
		return true;
	}
	return false;
}

template <class T>
inline bool chkmin(T &x, T y) {
	if (x > y) {
		x = y;
		return true;
	}
	return false;
}

template <class T>
inline void chkmax(T &x, std::initializer_list<T> Y) {
	for (auto &y : Y) chkmax(x, y);
}

template <class T>
inline void chkmin(T &x, std::initializer_list<T> Y) {
	for (auto &y : Y) chkmin(x, y);
}

template <class T, class V>
inline bool in_range(V v, T l, T r) {
	return l <= v && v <= r;
}

template <class T>
inline T isqrt(T x) {
	T res = std::sqrt(x);
	if (res * res > x) res--;
	return res;
}

template <>
inline u128 isqrt<u128>(u128 x) {
	if (x == 0) return 0;
	if (x < 4) return 1;
	u64 hi = static_cast<u64>(x >> 64);
	int bits = hi ? (128 - __builtin_clzll(hi))
	              : (64 - __builtin_clzll(static_cast<uint64_t>(x)));
	u128 g = static_cast<u128>(1) << ((bits + 1) / 2);

	// Newton-Raphson method：g' = (g + x / g) / 2
	for (u128 ng = (g + x / g) >> 1; ng < g;
	     g = std::exchange(ng, (ng + x / ng) >> 1));
	return g;
}

template <>
inline i128 isqrt<i128>(i128 x) {
	if (x < 0) return -1;
	return isqrt<u128>(static_cast<u128>(x));
}

inline i128 abs(i128 x) { return x < 0 ? -x : x; }

template <class T>
inline T ceil_div(T n, T m) {
	if (m < 0) m = -m, n = -n;
	if (n >= 0) {
		return (n + m - 1) / m;
	} else {
		return n / m;
	}
}

template <class T>
inline T floor_div(T n, T m) {
	if (m < 0) m = -m, n = -n;
	if (n >= 0) {
		return n / m;
	} else {
		return (n - m + 1) / m;
	}
}

template <class T>
std::vector<std::vector<T>> range_traverse(size_t n, T l, T r) {
	std::vector<T> cur(n);
	std::vector<std::vector<T>> res;
	auto dfs = [&](auto &self, size_t depth) {
		if (depth == n) {
			res.push_back(cur);
			return;
		}
		for (T i = l; i <= r; i++) {
			cur[depth] = i;
			self(self, depth + 1);
		}
	};
	dfs(dfs, 0);
	return res;
}
