#pragma once

#include "../core/bit.hpp"
#include "modint.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace fwt_detail {
template <class T>
inline void fmt_and(std::vector<T> &a, T c) {
	const int n = a.size();
	for (int d = 2; d <= n; d *= 2) {
		const int len = d >> 1;
		for (int i = 0; i < n; i += d) {
			for (int j = 0; j < len; j++) { a[i + j] += c * a[i + j + len]; }
		}
	}
}

template <class T>
inline void fmt_or(std::vector<T> &a, T c) {
	const int n = a.size();
	for (int d = 2; d <= n; d *= 2) {
		const int len = d >> 1;
		for (int i = 0; i < n; i += d) {
			for (int j = 0; j < len; j++) { a[i + j + len] += c * a[i + j]; }
		}
	}
}

template <uint32_t mod>
inline void fwt(std::vector<ModInt<mod>> &a, ModInt<mod> c) {
	ModInt<mod> tmp;
	const int n = a.size();
	for (int d = 2; d <= n; d *= 2) {
		const int len = d >> 1;
		for (int i = 0; i < n; i += d) {
			for (int j = 0; j < len; j++) {
				tmp = a[i + j];
				a[i + j] = (tmp + a[i + j + len]) * c;
				a[i + j + len] = (tmp - a[i + j + len]) * c;
			}
		}
	}
}

template <class T>
inline void fwt(std::vector<T> &a) {
	T tmp;
	const int n = a.size();
	for (int d = 2; d <= n; d *= 2) {
		const int len = d >> 1;
		for (int i = 0; i < n; i += d) {
			for (int j = 0; j < len; j++) {
				tmp = a[i + j];
				a[i + j] = tmp + a[i + j + len];
				a[i + j + len] = tmp - a[i + j + len];
			}
		}
	}
}

template <class T>
inline void ifwt(std::vector<T> &a) {
	T tmp;
	const int n = a.size();
	for (int d = 2; d <= n; d *= 2) {
		const int len = d >> 1;
		for (int i = 0; i < n; i += d) {
			for (int j = 0; j < len; j++) {
				tmp = a[i + j];
				a[i + j] = (tmp + a[i + j + len]) >> 1;
				a[i + j + len] = (tmp - a[i + j + len]) >> 1;
			}
		}
	}
}
} // namespace fwt_detail

template <class T>
inline std::vector<T> and_conv(std::vector<T> f, std::vector<T> g) {
	int len = ceil_pow2(std::max(f.size(), g.size()));
	f.resize(len), fwt_detail::fmt_and(f, T(1));
	g.resize(len), fwt_detail::fmt_and(g, T(1));
	for (int i = 0; i < len; i++) f[i] *= g[i];
	return fwt_detail::fmt_and(f, T(-1)), f;
}

template <class T>
inline std::vector<T> or_conv(std::vector<T> f, std::vector<T> g) {
	int len = ceil_pow2(std::max(f.size(), g.size()));
	f.resize(len), fwt_detail::fmt_or(f, T(1));
	g.resize(len), fwt_detail::fmt_or(g, T(1));
	for (int i = 0; i < len; i++) f[i] *= g[i];
	return fwt_detail::fmt_or(f, T(-1)), f;
}

template <uint32_t mod>
inline std::vector<ModInt<mod>>
xor_conv(std::vector<ModInt<mod>> f, std::vector<ModInt<mod>> g) {
	int len = ceil_pow2(std::max(f.size(), g.size()));
	f.resize(len), fwt_detail::fwt(f, ModInt<mod>(1));
	g.resize(len), fwt_detail::fwt(g, ModInt<mod>(1));
	for (int i = 0; i < len; i++) f[i] *= g[i];
	return fwt_detail::fwt(f, ModInt<mod>(2).inv()), f;
}

template <class T>
inline std::vector<T> xor_conv(std::vector<T> f, std::vector<T> g) {
	int len = ceil_pow2(std::max(f.size(), g.size()));
	f.resize(len), fwt_detail::fwt(f);
	g.resize(len), fwt_detail::fwt(g);
	for (int i = 0; i < len; i++) f[i] *= g[i];
	return fwt_detail::ifwt(f), f;
}
