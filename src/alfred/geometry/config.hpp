#pragma once

#include <cmath>
#include <type_traits>

template <class T>
struct Point;

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
