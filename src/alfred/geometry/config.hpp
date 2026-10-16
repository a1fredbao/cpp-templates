#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <type_traits>

template <class T>
struct Point;

#if defined(__SIZEOF_FLOAT128__) || defined(__FLT128_MANT_DIG__)

using f128 = __float128;

// libstdc++ has no arithmetic overloads for __float128.  Keep the common
// geometry-facing operations available without requiring libquadmath.
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

std::istream &operator>>(std::istream &is, f128 &p) {
	long double x;
	is >> x;
	p = static_cast<f128>(x);
	return is;
}
std::ostream &operator<<(std::ostream &os, const f128 &p) {
	return os << static_cast<long double>(p);
}

#endif

// Declaration needed by polar_cmp before point.hpp defines Point.
template <class T>
auto cross(const Point<T> &a, const Point<T> &b);

// User-tunable absolute tolerance.  It is intentionally non-constexpr so a
// problem can adjust it, for example:
//     eps<double> = 1e-10;
//     eps<long double> = 1e-12;
// A common starting point is 1e-9 for double and 1e-12 for long double.
// For integral T, T(1e-9) is zero and exact predicate code does not consult
// eps at all.
template <class T>
inline T eps = T(1e-9);

// Numeric 3-way comparison against zero.
// - Floating point: |x| <= eps<T> is treated as zero.
// - Integral types: exact comparison, eps is not used.
// This is an absolute tolerance.  For cross/dot products with large
// coordinates, scale the test according to the coordinate magnitude.
template <class T>
constexpr int sgn(T x) {
	if constexpr (std::is_floating_point_v<T>) {
		if (std::abs(x) <= eps<T>) return 0;
	}
	return (x > T(0)) - (x < T(0));
}

// Three-way comparison of a and b.
// Unsigned integers use direct comparisons to avoid wrap-around in a-b.
// Signed integral types use sgn(a-b) and therefore assume a-b does not
// overflow; compare manually for coordinates near the full type range.
template <class T>
constexpr int cmp(T a, T b) {
	if constexpr (std::is_unsigned_v<T>) {
		return (a > b) - (a < b);
	} else {
		return sgn(a - b);
	}
}

template <class T>
constexpr bool equal(T a, T b) {
	return cmp(a, b) == 0;
}

// Orders points by atan2(y, x) in (-pi, pi].
// The returned value is:
//   -1: y < 0
//    0: the origin or a point on the positive x-axis
//    1: y > 0 or a point on the negative x-axis
// Thus the negative x-axis is at +pi (the end of the order), matching the
// usual std::atan2(y, x) convention used by Library Checker.
template <class T>
constexpr int quad(const Point<T> &p) {
	int x = cmp(p.x, T(0)), y = cmp(p.y, T(0));
	if (y < 0) return -1;
	if (y == 0 && x >= 0) return 0;
	return 1;
}

// Strict angular comparator for std::sort.  It is exact for integral
// coordinates and uses cross(a, b) only inside one half-plane.
// Zero vectors and vectors with the same direction compare equivalent.
template <class T>
constexpr bool polar_cmp(const Point<T> &a, const Point<T> &b) {
	int qa = quad(a), qb = quad(b);
	if (qa != qb) return qa < qb;
	return sgn(cross(a, b)) > 0;
}
