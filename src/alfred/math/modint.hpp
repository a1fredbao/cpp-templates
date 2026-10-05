#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>

template <uint32_t M>
struct ModInt {
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i32 = int32_t;
	static_assert(1 < M && M < (1u << 31), "M must be in [2, 2^31)");

	static constexpr u32 calc_r() {
		u32 r = M;
		for (int i = 0; i < 4; i++) {
			r *= 2 - M * r;
		}
		return r;
	}
	static constexpr u32 R = calc_r();
	static constexpr u32 N2 = -u64(M) % M;

	u32 a;
	constexpr ModInt() : a(0) {}
	template <class T>
	constexpr ModInt(T x) : a(enc(norm(x))) {}

	template <class T>
	static constexpr u32 norm(T x) {
		if constexpr (std::is_signed_v<T>) {
			__int128_t v = static_cast<__int128_t>(x);
			v %= M;
			if (v < 0) v += M;
			return u32(v);
		} else {
			return u32(x % M);
		}
	}

	static constexpr u32 red(u64 b) {
		return (b + u64(u32(b) * u32(-R)) * M) >> 32;
	}
	static constexpr u32 enc(u32 x) {
		if constexpr (M & 1) {
			return red(u64(x) * N2);
		} else {
			return x;
		}
	}
	static constexpr u32 mul(u32 x, u32 y) {
		if constexpr (M & 1) {
			return red(u64(x) * y);
		} else {
			return u64(x) * y % M;
		}
	}
	constexpr u32 val() const {
		if constexpr (M & 1) {
			u32 x = red(a);
			return x >= M ? x - M : x;
		} else {
			return a;
		}
	}
	static constexpr u32 mod() { return M; }

	constexpr ModInt &operator+=(const ModInt &b) {
		if (i32(a += b.a - 2 * M) < 0) a += 2 * M;
		return *this;
	}
	constexpr ModInt &operator-=(const ModInt &b) {
		if (i32(a -= b.a) < 0) a += 2 * M;
		return *this;
	}
	constexpr ModInt &operator*=(const ModInt &b) {
		return a = mul(a, b.a), *this;
	}
	constexpr ModInt &operator/=(const ModInt &b) { return *this *= b.inv(); }
	friend constexpr ModInt operator+(ModInt a, const ModInt &b) {
		return a += b;
	}
	friend constexpr ModInt operator-(ModInt a, const ModInt &b) {
		return a -= b;
	}
	friend constexpr ModInt operator*(ModInt a, const ModInt &b) {
		return a *= b;
	}
	friend constexpr ModInt operator/(ModInt a, const ModInt &b) {
		return a /= b;
	}
	constexpr ModInt operator-() const { return ModInt() - *this; }
	constexpr ModInt operator+() const { return *this; }
	constexpr bool operator==(const ModInt &b) const {
		return val() == b.val();
	}
	constexpr bool operator!=(const ModInt &b) const {
		return val() != b.val();
	}

	constexpr ModInt pow(int64_t n) const {
		if (n < 0) return inv().pow(-n);
		ModInt r = 1, x = *this;
		while (n) {
			if (n & 1) r *= x;
			x *= x, n >>= 1;
		}
		return r;
	}
	static constexpr u32 inv_mod(u32 x) {
		int64_t a = x, b = M, u = 1, v = 0;
		while (b) {
			int64_t t = a / b;
			a -= t * b, u -= t * v;
			std::swap(a, b), std::swap(u, v);
		}
		assert(a == 1);
		return u32((u % M + M) % M);
	}
	// We may just use power(M - 2) in practice when M is prime.
	constexpr ModInt inv() const { return inv_mod(val()); }

	static constexpr ModInt primitive_root() {
		static_assert(M & 1, "primitive_root requires odd modulus");
		if constexpr (M == 998244353u) {
			return 3;
		} else if constexpr (M == 1000000007u) {
			return 5;
		} else {
			ModInt r = 2;
			while (r.pow((M - 1) / 2) == 1) r += 1;
			return r.pow((M - 1) >> __builtin_ctz(M - 1));
		}
	}

	friend std::ostream &operator<<(std::ostream &os, const ModInt &x) {
		return os << x.val();
	}
	friend std::istream &operator>>(std::istream &is, ModInt &x) {
		int64_t v;
		is >> v, x = v;
		return is;
	}
};

struct DynamicModInt {
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i32 = int32_t;

	static inline u32 M = 998244353, R, N2;
	static constexpr u32 calc_r(u32 m) {
		u32 r = m;
		for (int i = 0; i < 4; i++) r *= 2 - m * r;
		return r;
	}
	static void set_mod(u32 m) {
		assert(1 < m && m < (1u << 31));
		M = m;
		if (M & 1) {
			R = calc_r(m), N2 = -u64(m) % m;
		} else {
			R = N2 = 0;
		}
	}
	struct Init {
		Init() { set_mod(M); }
	};
	static inline Init init;

	u32 a;
	DynamicModInt() : a(0) {}
	template <class T>
	DynamicModInt(T x) : a(enc(norm(x))) {}

	template <class T>
	static u32 norm(T x) {
		if constexpr (std::is_signed_v<T>) {
			__int128_t v = static_cast<__int128_t>(x);
			v %= M;
			if (v < 0) v += M;
			return u32(v);
		} else {
			return u32(x % M);
		}
	}

	static u32 red(u64 b) { return (b + u64(u32(b) * u32(-R)) * M) >> 32; }
	static u32 enc(u32 x) { return (M & 1) ? red(u64(x) * N2) : x; }
	static u32 mul(u32 x, u32 y) {
		return (M & 1) ? red(u64(x) * y) : u32(u64(x) * y % M);
	}
	u32 val() const {
		if (M & 1) {
			u32 x = red(a);
			return x >= M ? x - M : x;
		} else {
			return a;
		}
	}
	static u32 mod() { return M; }

	DynamicModInt &operator+=(const DynamicModInt &b) {
		if (i32(a += b.a - 2 * M) < 0) a += 2 * M;
		return *this;
	}
	DynamicModInt &operator-=(const DynamicModInt &b) {
		if (i32(a -= b.a) < 0) a += 2 * M;
		return *this;
	}
	DynamicModInt &operator*=(const DynamicModInt &b) {
		a = mul(a, b.a);
		return *this;
	}
	DynamicModInt &operator/=(const DynamicModInt &b) {
		return *this *= b.inv();
	}
	friend DynamicModInt operator+(DynamicModInt a, const DynamicModInt &b) {
		return a += b;
	}
	friend DynamicModInt operator-(DynamicModInt a, const DynamicModInt &b) {
		return a -= b;
	}
	friend DynamicModInt operator*(DynamicModInt a, const DynamicModInt &b) {
		return a *= b;
	}
	friend DynamicModInt operator/(DynamicModInt a, const DynamicModInt &b) {
		return a /= b;
	}
	DynamicModInt operator-() const { return DynamicModInt() - *this; }
	DynamicModInt operator+() const { return *this; }
	bool operator==(const DynamicModInt &b) const { return val() == b.val(); }
	bool operator!=(const DynamicModInt &b) const { return val() != b.val(); }

	DynamicModInt pow(int64_t n) const {
		if (n < 0) return inv().pow(-n);
		DynamicModInt r = 1, x = *this;
		while (n) {
			if (n & 1) r *= x;
			x *= x, n >>= 1;
		}
		return r;
	}
	DynamicModInt inv() const {
		int64_t x = val(), y = M, u = 1, v = 0;
		while (y) {
			int64_t t = x / y;
			x -= t * y, u -= t * v;
			std::swap(x, y), std::swap(u, v);
		}
		return u;
	}

	static DynamicModInt primitive_root() {
		assert(M & 1);
		if (M == 998244353u) return 3;
		if (M == 1000000007u) return 5;
		DynamicModInt r = 2;
		while (r.pow((M - 1) / 2) == 1) r += 1;
		return r.pow((M - 1) >> __builtin_ctz(M - 1));
	}

	friend std::ostream &operator<<(std::ostream &os, const DynamicModInt &x) {
		return os << x.val();
	}
	friend std::istream &operator>>(std::istream &is, DynamicModInt &x) {
		int64_t v;
		is >> v, x = v;
		return is;
	}
};

using m998 = ModInt<998244353>;
using m107 = ModInt<1000000007>;
using dint = DynamicModInt;

template <class mint>
inline mint findPrimitiveRoot() {
	return mint::primitive_root();
}
