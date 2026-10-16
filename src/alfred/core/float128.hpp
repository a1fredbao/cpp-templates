#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <type_traits>

#if defined(__SIZEOF_FLOAT128__) || defined(__FLT128_MANT_DIG__)

#define AF_HAS_FLOAT128 1

using f128 = __float128;

// libstdc++ has no arithmetic overloads for __float128.  Keep the common
// floating-point operations available without requiring libquadmath.
namespace std {
inline f128 abs(f128 x) { return x < 0 ? -x : x; }
inline f128 fabs(f128 x) { return abs(x); }
inline f128 sqrt(f128 x) {
	if (x == 0) return x;
	f128 r = static_cast<f128>(
	    std::sqrt(static_cast<long double>(x))
	);
	if (!(r > 0)) return r;
	for (int i = 0; i < 4; i++) {
		r = (r + x / r) / 2;
	}
	return r;
}
inline f128 floor(f128 x) {
	return static_cast<f128>(std::floor(static_cast<long double>(x)));
}
inline f128 ceil(f128 x) {
	return static_cast<f128>(std::ceil(static_cast<long double>(x)));
}
inline f128 trunc(f128 x) {
	return static_cast<f128>(std::trunc(static_cast<long double>(x)));
}
inline f128 round(f128 x) {
	return static_cast<f128>(std::round(static_cast<long double>(x)));
}
inline f128 fmod(f128 x, f128 y) {
	return static_cast<f128>(
	    std::fmod(static_cast<long double>(x), static_cast<long double>(y))
	);
}

#if !defined(_GLIBCXX_USE_FLOAT128)
__extension__ template <>
struct numeric_limits<f128> {
	static constexpr bool is_specialized = true;
	static constexpr f128 min() noexcept { return __FLT128_MIN__; }
	static constexpr f128 max() noexcept { return __FLT128_MAX__; }
	static constexpr f128 lowest() noexcept { return -__FLT128_MAX__; }
	static constexpr int digits = __FLT128_MANT_DIG__;
	static constexpr int digits10 = __FLT128_DIG__;
	static constexpr int max_digits10 = __FLT128_DECIMAL_DIG__;
	static constexpr bool is_signed = true;
	static constexpr bool is_integer = false;
	static constexpr bool is_exact = false;
	static constexpr int radix = 2;
	static constexpr f128 epsilon() noexcept { return __FLT128_EPSILON__; }
	static constexpr f128 round_error() noexcept { return f128(0.5); }
	static constexpr int min_exponent = __FLT128_MIN_EXP__;
	static constexpr int min_exponent10 = __FLT128_MIN_10_EXP__;
	static constexpr int max_exponent = __FLT128_MAX_EXP__;
	static constexpr int max_exponent10 = __FLT128_MAX_10_EXP__;
	static constexpr bool has_infinity = true;
	static constexpr bool has_quiet_NaN = true;
	static constexpr bool has_signaling_NaN = true;
	static constexpr float_denorm_style has_denorm = denorm_present;
	static constexpr bool has_denorm_loss = false;
	static constexpr f128 infinity() noexcept {
		return __builtin_huge_valf128();
	}
	static constexpr f128 quiet_NaN() noexcept {
		return __builtin_nanf128("");
	}
	static constexpr f128 signaling_NaN() noexcept {
		return __builtin_nansf128("");
	}
	static constexpr f128 denorm_min() noexcept {
		return __FLT128_DENORM_MIN__;
	}
	static constexpr bool is_iec559 = true;
	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = false;
	static constexpr bool traps = false;
	static constexpr bool tinyness_before = false;
	static constexpr float_round_style round_style = round_to_nearest;
};
#endif
} // namespace std

inline std::istream &operator>>(std::istream &is, f128 &p) {
	long double x;
	is >> x;
	p = static_cast<f128>(x);
	return is;
}
inline std::ostream &operator<<(std::ostream &os, const f128 &p) {
	return os << static_cast<long double>(p);
}

#else

#define AF_HAS_FLOAT128 0

#endif
