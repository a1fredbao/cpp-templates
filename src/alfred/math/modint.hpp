#pragma once

#include "../core/types.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <type_traits>

namespace montgomery_detail {

// Returns -m^{-1} modulo 2^32. m must be odd.
constexpr uint32_t inv_mod_pow2(uint32_t m) noexcept {
	uint32_t x = 1;
	for (int i = 0; i < 5; i++) {
		x *= 2u - m * x;
	}
	return 0u - x;
}

// Returns 2^32 mod m.
constexpr uint32_t r_mod(uint32_t m) noexcept {
	return static_cast<uint32_t>((uint64_t(1) << 32) % m);
}

// Returns (2^32)^2 mod m.
constexpr uint32_t r2_mod(uint32_t m) noexcept {
	uint64_t r = r_mod(m);
	return static_cast<uint32_t>(r * r % m);
}

constexpr int ctz(uint32_t x) noexcept {
	int count = 0;
	while ((x & 1u) == 0) {
		x >>= 1;
		count++;
	}
	return count;
}

inline uint32_t inverse(uint32_t value, uint32_t mod) {
	int64_t a = value, b = mod;
	int64_t x = 1, y = 0;
	while (b != 0) {
		int64_t q = a / b;
		a -= q * b;
		std::swap(a, b);
		x -= q * y;
		std::swap(x, y);
	}
	assert(a == 1);
	x %= mod;
	if (x < 0) x += mod;
	return static_cast<uint32_t>(x);
}

} // namespace montgomery_detail

// Montgomery-form modular integer for a compile-time odd modulus below 2^31.
// v is the raw Montgomery residue; use val() for the ordinary value.
template <uint32_t M>
class ModInt {
	static_assert(M > 1u, "modulus must be greater than one");
	static_assert(M < (uint32_t(1) << 31), "use a 32-bit modulus below 2^31");
	static_assert(
	    M % 2u == 1u, "Montgomery multiplication requires odd modulus"
	);

public:
	using value_type = uint32_t;

	static constexpr value_type mod() noexcept { return M; }
	template <class T>
	static constexpr value_type norm(T x) noexcept {
		return normalize(x);
	}
	static constexpr ModInt sgn(int t) noexcept {
		return t & 1 ? ModInt(M - 1) : ModInt(1);
	}
	static constexpr value_type montgomery_one() noexcept {
		return montgomery_detail::r_mod(M);
	}

	value_type v;

	constexpr ModInt() noexcept : v(0) {}

	template <
	    class T, std::enable_if_t<
	                 is_integral<std::remove_cv_t<T>>::value &&
	                     !std::is_same_v<std::remove_cv_t<T>, bool>,
	                 int> = 0>
	constexpr ModInt(T x) noexcept : v(to_mont(normalize(x))) {}

	static constexpr ModInt raw(value_type x) noexcept {
		ModInt result;
		result.v = x;
		return result;
	}

	static constexpr ModInt zero() noexcept { return ModInt(); }
	static constexpr ModInt one() noexcept { return ModInt(1); }

	constexpr value_type raw_value() const noexcept { return v; }
	constexpr value_type val() const noexcept { return from_mont(v); }
	explicit constexpr operator value_type() const noexcept { return val(); }

	constexpr bool is_zero() const noexcept { return v == 0; }
	constexpr bool is_one() const noexcept { return val() == 1; }

	constexpr ModInt operator+() const noexcept { return *this; }
	constexpr ModInt operator-() const noexcept {
		return raw(v == 0 ? 0 : M - v);
	}

	constexpr ModInt &operator++() noexcept {
		v++;
		if (v == M) v = 0;
		return *this;
	}
	constexpr ModInt operator++(int) noexcept {
		ModInt result = *this;
		++*this;
		return result;
	}
	constexpr ModInt &operator--() noexcept {
		if (v == 0) v = M;
		v--;
		return *this;
	}
	constexpr ModInt operator--(int) noexcept {
		ModInt result = *this;
		--*this;
		return result;
	}

	constexpr ModInt &operator+=(const ModInt &rhs) noexcept {
		v += rhs.v;
		if (v >= M) v -= M;
		return *this;
	}
	constexpr ModInt &operator-=(const ModInt &rhs) noexcept {
		v -= rhs.v;
		if (v >= M) v += M;
		return *this;
	}
	constexpr ModInt &operator*=(const ModInt &rhs) noexcept {
		v = mul_raw(v, rhs.v);
		return *this;
	}
	ModInt &operator/=(const ModInt &rhs) { return *this *= rhs.inv(); }

	friend constexpr ModInt operator+(ModInt lhs, const ModInt &rhs) noexcept {
		return lhs += rhs;
	}
	friend constexpr ModInt operator-(ModInt lhs, const ModInt &rhs) noexcept {
		return lhs -= rhs;
	}
	friend constexpr ModInt operator*(ModInt lhs, const ModInt &rhs) noexcept {
		return lhs *= rhs;
	}
	friend ModInt operator/(ModInt lhs, const ModInt &rhs) {
		return lhs /= rhs;
	}

	friend constexpr bool
	operator==(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.v == rhs.v;
	}
	friend constexpr bool
	operator!=(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.v != rhs.v;
	}
	friend constexpr bool
	operator<(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.val() < rhs.val();
	}
	friend constexpr bool
	operator<=(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.val() <= rhs.val();
	}
	friend constexpr bool
	operator>(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.val() > rhs.val();
	}
	friend constexpr bool
	operator>=(const ModInt &lhs, const ModInt &rhs) noexcept {
		return lhs.val() >= rhs.val();
	}

	constexpr ModInt sqr() const noexcept { return *this * *this; }

	constexpr ModInt pow_unsigned(uint64_t exponent) const noexcept {
		ModInt result = one();
		ModInt base = *this;
		while (exponent != 0) {
			if (exponent & 1) result *= base;
			base *= base;
			exponent >>= 1;
		}
		return result;
	}

	ModInt pow(int64_t exponent) const {
		if (exponent < 0) {
			uint64_t positive = uint64_t(-(exponent + 1)) + 1;
			return inv().pow_unsigned(positive);
		}
		return pow_unsigned(uint64_t(exponent));
	}

	ModInt inv() const {
		assert(v != 0);
		return raw(to_mont(montgomery_detail::inverse(val(), M)));
	}

	static constexpr ModInt primitive_root() {
		if constexpr (M == 998244353u) {
			return ModInt(3);
		} else if constexpr (M == 1000000007u) {
			return ModInt(5);
		} else {
			const uint32_t p = M;
			ModInt root = 2;
			while (root.pow_unsigned((p - 1) / 2) == one()) {
				root += 1;
			}
			return root.pow_unsigned((p - 1) >> montgomery_detail::ctz(p - 1));
		}
	}

	friend std::ostream &operator<<(std::ostream &os, const ModInt &x) {
		return os << x.val();
	}
	friend std::istream &operator>>(std::istream &is, ModInt &x) {
		int64_t value;
		is >> value;
		x = ModInt(value);
		return is;
	}

private:
	static constexpr value_type mont_inv = montgomery_detail::inv_mod_pow2(M);
	static constexpr value_type mont_r2 = montgomery_detail::r2_mod(M);

	static constexpr value_type mul_raw(value_type a, value_type b) noexcept {
		uint64_t product = uint64_t(a) * b;
		value_type m = value_type(product) * mont_inv;
		uint64_t reduced = (product + uint64_t(m) * M) >> 32;
		if (reduced >= M) reduced -= M;
		return value_type(reduced);
	}

	static constexpr value_type to_mont(value_type x) noexcept {
		return mul_raw(x, mont_r2);
	}

	static constexpr value_type from_mont(value_type x) noexcept {
		return mul_raw(x, 1);
	}

	template <class T>
	static constexpr value_type normalize(T x) noexcept {
		using U = std::remove_cv_t<T>;
		using W = std::conditional_t<(sizeof(U) < sizeof(int64_t)), int64_t, U>;
		W value = static_cast<W>(x);
		if constexpr (is_signed_int<U>::value) {
			W remainder = value % static_cast<W>(M);
			if (remainder < 0) remainder += static_cast<W>(M);
			return static_cast<value_type>(remainder);
		} else {
			return static_cast<value_type>(value % static_cast<W>(M));
		}
	}
};

// Runtime-modulus companion. Call set_mod once before using this type.
// The modulus must be odd and below 2^31.
class DynamicModInt {
public:
	using value_type = uint32_t;

	static value_type mod() noexcept { return mod_; }
	template <class T>
	static value_type norm(T x) noexcept {
		return normalize(x);
	}
	static DynamicModInt sgn(int t) noexcept {
		return t & 1 ? DynamicModInt(mod_ - 1) : DynamicModInt(1);
	}

	static void set_mod(value_type m) {
		assert(m > 1u);
		assert(m < (value_type(1) << 31));
		assert(m % 2u == 1u);
		mod_ = m;
		inv_ = montgomery_detail::inv_mod_pow2(m);
		r2_ = montgomery_detail::r2_mod(m);
		one_ = montgomery_detail::r_mod(m);
	}

	static value_type montgomery_one() noexcept { return one_; }

	value_type v;

	DynamicModInt() noexcept : v(0) {}

	template <
	    class T, std::enable_if_t<
	                 is_integral<std::remove_cv_t<T>>::value &&
	                     !std::is_same_v<std::remove_cv_t<T>, bool>,
	                 int> = 0>
	DynamicModInt(T x) noexcept : v(to_mont(normalize(x))) {}

	static DynamicModInt raw(value_type x) noexcept {
		DynamicModInt result;
		result.v = x;
		return result;
	}

	static DynamicModInt zero() noexcept { return DynamicModInt(); }
	static DynamicModInt one() noexcept { return DynamicModInt(1); }

	value_type raw_value() const noexcept { return v; }
	value_type val() const noexcept { return from_mont(v); }
	explicit operator value_type() const noexcept { return val(); }

	bool is_zero() const noexcept { return v == 0; }
	bool is_one() const noexcept { return val() == 1; }

	DynamicModInt operator+() const noexcept { return *this; }
	DynamicModInt operator-() const noexcept {
		return raw(v == 0 ? 0 : mod_ - v);
	}

	DynamicModInt &operator++() noexcept {
		if (++v == mod_) v = 0;
		return *this;
	}
	DynamicModInt operator++(int) noexcept {
		DynamicModInt result = *this;
		++*this;
		return result;
	}
	DynamicModInt &operator--() noexcept {
		if (v == 0) v = mod_;
		v--;
		return *this;
	}
	DynamicModInt operator--(int) noexcept {
		DynamicModInt result = *this;
		--*this;
		return result;
	}

	DynamicModInt &operator+=(const DynamicModInt &rhs) noexcept {
		v += rhs.v;
		if (v >= mod_) v -= mod_;
		return *this;
	}
	DynamicModInt &operator-=(const DynamicModInt &rhs) noexcept {
		v -= rhs.v;
		if (v >= mod_) v += mod_;
		return *this;
	}
	DynamicModInt &operator*=(const DynamicModInt &rhs) noexcept {
		v = mul_raw(v, rhs.v);
		return *this;
	}
	DynamicModInt &operator/=(const DynamicModInt &rhs) {
		return *this *= rhs.inv();
	}

	friend DynamicModInt
	operator+(DynamicModInt lhs, const DynamicModInt &rhs) noexcept {
		return lhs += rhs;
	}
	friend DynamicModInt
	operator-(DynamicModInt lhs, const DynamicModInt &rhs) noexcept {
		return lhs -= rhs;
	}
	friend DynamicModInt
	operator*(DynamicModInt lhs, const DynamicModInt &rhs) noexcept {
		return lhs *= rhs;
	}
	friend DynamicModInt
	operator/(DynamicModInt lhs, const DynamicModInt &rhs) {
		return lhs /= rhs;
	}

	friend bool
	operator==(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.v == rhs.v;
	}
	friend bool
	operator!=(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.v != rhs.v;
	}
	friend bool
	operator<(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.val() < rhs.val();
	}
	friend bool
	operator<=(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.val() <= rhs.val();
	}
	friend bool
	operator>(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.val() > rhs.val();
	}
	friend bool
	operator>=(const DynamicModInt &lhs, const DynamicModInt &rhs) noexcept {
		return lhs.val() >= rhs.val();
	}

	DynamicModInt sqr() const noexcept { return *this * *this; }

	DynamicModInt pow_unsigned(uint64_t exponent) const noexcept {
		DynamicModInt result = one();
		DynamicModInt base = *this;
		while (exponent != 0) {
			if (exponent & 1) result *= base;
			base *= base;
			exponent >>= 1;
		}
		return result;
	}

	DynamicModInt pow(int64_t exponent) const {
		if (exponent < 0) {
			uint64_t positive = uint64_t(-(exponent + 1)) + 1;
			return inv().pow_unsigned(positive);
		}
		return pow_unsigned(uint64_t(exponent));
	}

	DynamicModInt inv() const {
		assert(v != 0);
		return raw(to_mont(montgomery_detail::inverse(val(), mod_)));
	}

	static DynamicModInt primitive_root() {
		const value_type p = mod_;
		if (p == 998244353u) return DynamicModInt(3);
		if (p == 1000000007u) return DynamicModInt(5);
		DynamicModInt root = 2;
		while (root.pow_unsigned((p - 1) / 2) == one()) {
			root += 1;
		}
		return root.pow_unsigned((p - 1) >> montgomery_detail::ctz(p - 1));
	}

	friend std::ostream &operator<<(std::ostream &os, const DynamicModInt &x) {
		return os << x.val();
	}
	friend std::istream &operator>>(std::istream &is, DynamicModInt &x) {
		int64_t value;
		is >> value;
		x = DynamicModInt(value);
		return is;
	}

private:
	static inline value_type mod_ = 998244353u;
	static inline value_type inv_ = montgomery_detail::inv_mod_pow2(998244353u);
	static inline value_type r2_ = montgomery_detail::r2_mod(998244353u);
	static inline value_type one_ = montgomery_detail::r_mod(998244353u);

	static value_type mul_raw(value_type a, value_type b) noexcept {
		uint64_t product = uint64_t(a) * b;
		value_type m = value_type(product) * inv_;
		uint64_t reduced = (product + uint64_t(m) * mod_) >> 32;
		if (reduced >= mod_) reduced -= mod_;
		return value_type(reduced);
	}

	static value_type to_mont(value_type x) noexcept { return mul_raw(x, r2_); }

	static value_type from_mont(value_type x) noexcept { return mul_raw(x, 1); }

	template <class T>
	static value_type normalize(T x) noexcept {
		using U = std::remove_cv_t<T>;
		using W = std::conditional_t<(sizeof(U) < sizeof(int64_t)), int64_t, U>;
		W value = static_cast<W>(x);
		if constexpr (is_signed_int<U>::value) {
			W remainder = value % static_cast<W>(mod_);
			if (remainder < 0) remainder += static_cast<W>(mod_);
			return static_cast<value_type>(remainder);
		} else {
			return static_cast<value_type>(value % static_cast<W>(mod_));
		}
	}
};

using m998 = ModInt<998244353>;
using m107 = ModInt<1000000007>;
using dint = DynamicModInt;

template <class mint>
inline mint findPrimitiveRoot() {
	return mint::primitive_root();
}

template <uint32_t M>
void __print(ModInt<M> x) {
	std::cerr << x;
}

inline void __print(DynamicModInt x) { std::cerr << x; }

namespace std {

template <uint32_t M>
struct hash<ModInt<M>> {
	size_t operator()(const ModInt<M> &x) const noexcept { return x.val(); }
};

template <>
struct hash<DynamicModInt> {
	size_t operator()(const DynamicModInt &x) const noexcept { return x.val(); }
};

} // namespace std
