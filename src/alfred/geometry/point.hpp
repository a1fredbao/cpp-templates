#pragma once

#include "../core/types.hpp"
#include "config.hpp"

#include <cmath>
#include <iostream>
#include <type_traits>

// A point and a vector are intentionally the same type.  This follows the
// library design: geometric code is written in terms of position vectors,
// and a separate Vec class would only add conversions in contest code.
template <class T>
struct Point {
	using Vec = Point<T>;

	T x{}, y{};
	Point() = default;
	Point(T x_, T y_) : x(x_), y(y_) {}

	// Implicit conversion is deliberate.  It makes
	// Point<long double> q = p; convenient in mixed-precision code.
	// Converting to an integral T truncates as usual.
	template <class U>
	Point(const Point<U> &p) : x(T(p.x)), y(T(p.y)) {}

	// Arithmetic follows T directly (including integer division).  Use
	// dot/cross/square when exact widened integer products are needed.
	Point &operator+=(const Point &p) {
		x += p.x, y += p.y;
		return *this;
	}
	Point &operator-=(const Point &p) {
		x -= p.x, y -= p.y;
		return *this;
	}
	Point &operator*=(const T &v) {
		x *= v, y *= v;
		return *this;
	}
	Point &operator/=(const T &v) {
		x /= v, y /= v;
		return *this;
	}

	friend Point operator+(Point a, const Point &b) { return a += b; }
	friend Point operator-(Point a, const Point &b) { return a -= b; }
	friend Point operator-(const Point &a) { return Point(-a.x, -a.y); }
	friend Point operator*(Point a, const T &b) { return a *= b; }
	friend Point operator*(const T &a, Point b) { return b *= a; }
	friend Point operator/(Point a, const T &b) { return a /= b; }
	friend bool operator==(const Point &a, const Point &b) {
		return a.x == b.x && a.y == b.y;
	}
	friend bool operator!=(const Point &a, const Point &b) { return !(a == b); }
	friend bool operator<(const Point &a, const Point &b) {
		return a.x < b.x || (a.x == b.x && a.y < b.y);
	}
	friend std::istream &operator>>(std::istream &is, Point &p) {
		return is >> p.x >> p.y;
	}
	// Stream output uses the format "(x, y)".
	friend std::ostream &operator<<(std::ostream &os, const Point &p) {
		return os << '(' << p.x << ", " << p.y << ')';
	}
};

template <class T>
using Vec = Point<T>;

// Type used by exact algebraic predicates.
// For integral T it is i128, so dot/cross of long long coordinates do not
// overflow in the common contest range.  For floating T it is T itself.
// If T is already i128, products must still fit in i128.
template <class T>
using scalar_t = std::conditional_t<is_integral<T>::value, i128, T>;

// Type used by metric operations that may need division or sqrt.
// Integral coordinates are promoted to long double; floating types are kept.
template <class T>
using calc_t = std::conditional_t<std::is_floating_point_v<T>, T, long double>;

// Dot product with exact widening for integral coordinates.
template <class T>
scalar_t<T> dot(const Point<T> &a, const Point<T> &b) {
	using R = scalar_t<T>;
	return R(a.x) * R(b.x) + R(a.y) * R(b.y);
}

// 2D cross product a.x*b.y - a.y*b.x.
// The sign means "b is on which side of a"; positive means b is
// counterclockwise from a.
template <class T>
auto cross(const Point<T> &a, const Point<T> &b) {
	if constexpr (is_integral<T>::value) {
		using R = i128;
		return R(a.x) * R(b.y) - R(a.y) * R(b.x);
	} else {
		return a.x * b.y - a.y * b.x;
	}
}

// Cross product of (b-a) and (c-a), i.e. orientation(a,b,c).
template <class T>
scalar_t<T> cross(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
	return cross(b - a, c - a);
}

// Squared Euclidean norm.  Prefer this over length() when only comparisons
// are needed, because it avoids sqrt and is exact for integral coordinates.
template <class T>
scalar_t<T> square(const Point<T> &p) {
	return dot(p, p);
}

// Euclidean norm as a floating-point value.
template <class T>
calc_t<T> length(const Point<T> &p) {
	return std::sqrt(static_cast<calc_t<T>>(square(p)));
}

// Euclidean distance between two points.
template <class T>
calc_t<T> dist(const Point<T> &a, const Point<T> &b) {
	return length(a - b);
}

// Unit vector.  The zero vector is not handled; normalizing it divides by 0.
template <class T>
Point<calc_t<T>> normalize(const Point<T> &p) {
	using U = calc_t<T>;
	return Point<U>(p) / length(p);
}

// Rotate counterclockwise by rad radians.  The result type follows calc_t,
// so integer input produces a floating-point point.
template <class T>
Point<calc_t<T>> rotate(const Point<T> &p, long double rad) {
	using U = calc_t<T>;
	U c = std::cos(rad), s = std::sin(rad);
	return Point<U>(p.x * c - p.y * s, p.x * s + p.y * c);
}

// Exact counterclockwise rotation by 90 degrees: (x,y) -> (-y,x).
template <class T>
Point<T> rotate_90(const Point<T> &p) {
	return Point<T>(-p.y, p.x);
}
