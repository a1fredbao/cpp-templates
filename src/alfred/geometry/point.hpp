#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <vector>

#ifndef AFMT_FLAT_GEOMETRY_EPS
#define AFMT_FLAT_GEOMETRY_EPS 1e-9
#endif

// TODO(alfred): replace this local epsilon with the geometry-owned constants
// after the computational geometry module is rewritten.

// Helper to handle floating point comparisons safely
template <class T>
constexpr int sgn(T x, double eps = AFMT_FLAT_GEOMETRY_EPS) {
	if constexpr (std::is_floating_point_v<T>) {
		return (x > eps) - (x < -eps);
	} else {
		return (x > 0) - (x < 0);
	}
}

template <class T>
struct Vec {
	T x, y;
	explicit constexpr Vec(T _x = 0, T _y = 0) : x(_x), y(_y) {}

	constexpr T norm2() const { return x * x + y * y; }
	inline double norm() const {
		return std::sqrt(static_cast<double>(norm2()));
	}

	constexpr Vec operator+(const Vec &rhs) const {
		return Vec(x + rhs.x, y + rhs.y);
	}
	constexpr Vec operator-(const Vec &rhs) const {
		return Vec(x - rhs.x, y - rhs.y);
	}
	constexpr Vec operator*(T d) const { return Vec(x * d, y * d); }
	constexpr Vec operator/(T d) const { return Vec(x / d, y / d); }

	constexpr Vec &operator+=(const Vec &rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	constexpr Vec &operator-=(const Vec &rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	constexpr Vec &operator*=(T d) {
		x *= d;
		y *= d;
		return *this;
	}
	constexpr Vec &operator/=(T d) {
		x /= d;
		y /= d;
		return *this;
	}

	constexpr Vec operator+() const { return *this; }
	constexpr Vec operator-() const { return Vec(-x, -y); }

	constexpr bool operator==(const Vec &rhs) const {
		return sgn(x - rhs.x) == 0 && sgn(y - rhs.y) == 0;
	}
	constexpr bool operator!=(const Vec &rhs) const { return !(*this == rhs); }

	// Strict weak ordering for std::sort (avoids eps to ensure transitivity)
	constexpr bool operator<(const Vec &rhs) const {
		if (x != rhs.x) return x < rhs.x;
		return y < rhs.y;
	}

	friend constexpr Vec operator*(T d, const Vec &v) { return v * d; }

	friend std::ostream &operator<<(std::ostream &os, const Vec &v) {
		return os << "(" << v.x << ", " << v.y << ")";
	}
	friend std::istream &operator>>(std::istream &is, Vec &v) {
		return is >> v.x >> v.y;
	}
};

template <class T>
using Point = Vec<T>;

template <class T>
constexpr T dot(const Vec<T> &a, const Vec<T> &b) {
	return a.x * b.x + a.y * b.y;
}

template <class T>
constexpr T cross(const Vec<T> &a, const Vec<T> &b) {
	return a.x * b.y - a.y * b.x;
}

template <class T>
constexpr T cross(const Point<T> &o, const Point<T> &a, const Point<T> &b) {
	return cross(a - o, b - o);
}

// Rotates a vector counter-clockwise by `angle` (in radians)
template <class T>
inline Vec<double> rotate(const Vec<T> &v, double angle) {
	double c = std::cos(angle), s = std::sin(angle);
	return Vec<double>(v.x * c - v.y * s, v.x * s + v.y * c);
}

// Rotates a vector 90 degrees counter-clockwise
template <class T>
constexpr Vec<T> rotate90(const Vec<T> &v) {
	return Vec<T>(-v.y, v.x);
}

template <class T>
struct PolarAngleComparator {
	Point<T> o;
	Vec<T> base;

	explicit PolarAngleComparator(
	    const Point<T> &o = Point<T>(0, 0), const Vec<T> &base = Vec<T>(1, 0)
	)
	    : o(o), base(base) {}

	inline int get_half(const Vec<T> &v) const {
		int c = sgn(cross(base, v));
		if (c > 0) return 0;
		if (c < 0) return 1;
		return sgn(dot(base, v)) < 0;
	}

	inline bool operator()(const Point<T> &a, const Point<T> &b) const {
		Vec<T> va = a - o, vb = b - o;
		int ha = get_half(va), hb = get_half(vb);
		if (ha != hb) return ha < hb;
		int c = sgn(cross(va, vb));
		if (c != 0) return c > 0;
		return va.norm2() < vb.norm2();
	}
};

template <class T>
struct Line {
	Point<T> a, b;
	Line() : a(), b() {}
	Line(Point<T> a, Point<T> b) : a(a), b(b) {}
	Vec<T> dir() const { return b - a; }
};

template <class T>
struct Segment : Line<T> {
	using Line<T>::Line;
};

// Projection of point p onto line l
template <class T>
Point<double> projection(const Point<T> &p, const Line<T> &l) {
	Vec<T> d = l.dir();
	double t = static_cast<double>(dot(p - l.a, d)) / d.norm2();
	return Point<double>(l.a.x + d.x * t, l.a.y + d.y * t);
}

// Reflection of point p across line l
template <class T>
Point<double> reflection(const Point<T> &p, const Line<T> &l) {
	Point<double> proj = projection(p, l);
	return Point<double>(2.0 * proj.x - p.x, 2.0 * proj.y - p.y);
}

// Distance from point p to line l
template <class T>
double distance(const Point<T> &p, const Line<T> &l) {
	return std::abs(cross(p - l.a, l.dir())) / l.dir().norm();
}

// Polygon area
template <class T>
double polygon_area(const std::vector<Point<T>> &poly) {
	double res = 0;
	for (size_t i = 0, n = poly.size(); i < n; ++i) {
		res += static_cast<double>(cross(poly[i], poly[(i + 1) % n]));
	}
	return std::abs(res) / 2.0;
}
