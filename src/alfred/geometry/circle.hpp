#pragma once

#include "polygon.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

// Circle with center p and radius r.  r may be negative as a sentinel in
// circumcircle(), where -1 means "the three points are collinear".
template <class T>
struct Circle {
	Point<T> p{};
	T r{};
	Circle() = default;
	Circle(Point<T> p_, T r_) : p(p_), r(r_) {}
};

// Closed-disk containment with a small relative tolerance on the radius.
// Points on the boundary are considered inside.
template <class T>
bool inside_circle(const Point<T> &p, const Circle<T> &c) {
	using U = calc_t<T>;
	U r = U(c.r);
	U tol = U(16) * eps<U> * std::max<U>(1, r);
	U d = dist(p, c.p);
	return d <= r + tol;
}

// Intersection of a circle and the infinite line l.
// Returns 0, 1 (tangent), or 2 points.  For a zero-length line, the single
// point is returned only when it lies on the circle.  The two-point result
// is ordered along the line direction.
template <class T>
std::vector<Point<calc_t<T>>>
circle_line_intersection(const Circle<T> &c, const Line<T> &l) {
	using U = calc_t<T>;
	Point<U> a(l.a), b(l.b), o(c.p);
	Point<U> d = b - a;
	U l2 = square(d);
	if (sgn(l2) == 0) {
		return sgn(dist(o, a) - U(c.r)) == 0 ? std::vector<Point<U>>{a}
		                                     : std::vector<Point<U>>{};
	}
	Point<U> proj = a + d * (dot(o - a, d) / l2);
	U h2 = U(c.r) * U(c.r) - square(proj - o);
	U tol = U(16) * eps<U> * std::max<U>(1, U(c.r) * U(c.r));
	if (h2 < -tol) return {};
	if (std::abs(h2) <= tol) return {proj};
	Point<U> dir = d / std::sqrt(l2);
	U h = std::sqrt(h2);
	return {proj - dir * h, proj + dir * h};
}

// Intersection of a circle and the closed segment l.
// This filters circle_line_intersection() to points inside the segment.
template <class T>
std::vector<Point<calc_t<T>>>
circle_segment_intersection(const Circle<T> &c, const Line<T> &l) {
	using U = calc_t<T>;
	std::vector<Point<U>> res;
	for (const auto &p : circle_line_intersection(c, l)) {
		if (point_on_segment(p, Line<U>(Point<U>(l.a), Point<U>(l.b)))) {
			res.push_back(p);
		}
	}
	return res;
}

// Intersection of two circles.
// Returns 0 points for disjoint circles, 1 point for tangency, or 2 points
// for a proper intersection.  Concentric circles with equal radius return {}
// because there are infinitely many intersections.  The two-point result is
// ordered consistently but not lexicographically.
template <class T>
std::vector<Point<calc_t<T>>>
circle_circle_intersection(const Circle<T> &c1, const Circle<T> &c2) {
	using U = calc_t<T>;
	Point<U> a(c1.p), b(c2.p);
	U r1 = c1.r, r2 = c2.r, d = dist(a, b);
	U tol = U(16) * eps<U> * std::max({U(1), r1, r2, d});
	if (d <= tol && std::abs(r1 - r2) <= tol) return {};
	if (d > r1 + r2 + tol || d < std::abs(r1 - r2) - tol) return {};

	U u = (r1 * r1 - r2 * r2 + d * d) / (U(2) * d);
	U h2 = r1 * r1 - u * u;
	Point<U> o = a + (b - a) * (u / d);
	if (h2 <= tol) return {o};
	Point<U> dir = rotate_90(b - a) * (std::sqrt(h2) / d);
	return {o + dir, o - dir};
}

// Tangent points from a point p to a circle.
// Returns 0 points if p is strictly inside, 1 point if p is on the circle,
// and 2 points if p is outside.  A zero-radius circle is degenerate; the
// algebraic branch is still executed, so callers should avoid that case
// unless they intentionally want the raw formula.
template <class T>
std::vector<Point<calc_t<T>>>
tangent_points(const Point<T> &p, const Circle<T> &c) {
	using U = calc_t<T>;
	Point<U> q(p), o(c.p);
	U r = c.r;
	Point<U> v = q - o;
	U d2 = square(v);
	U tol = U(16) * eps<U> * std::max<U>(1, r * r);
	if (d2 < r * r - tol) return {};
	if (std::abs(d2 - r * r) <= tol) return {q};
	Point<U> x = o + v * (r * r / d2);
	Point<U> dir = rotate_90(v) * (std::sqrt(d2 - r * r) / d2);
	return {x + dir, x - dir};
}

// Common tangent lines of two circles.
// The result contains up to four lines: external tangents are emitted first,
// then internal tangents.  Lines are represented by a tangent point and a
// direction, and are not normalized.  Concentric circles return {} because
// either there are no tangents or infinitely many when the circles coincide.
// At tangency the tangent that collapses to one line is emitted once; if one
// circle is strictly inside the other, no tangent is produced.  Circles with
// zero radius are outside the intended domain.
template <class T>
std::vector<Line<calc_t<T>>>
common_tangents(const Circle<T> &c1, const Circle<T> &c2) {
	using U = calc_t<T>;
	std::vector<Line<U>> res;
	Point<U> a(c1.p), b(c2.p);
	U r1 = c1.r, r2 = c2.r, d = dist(a, b);
	U tol = U(16) * eps<U> * std::max({U(1), r1, r2, d});
	if (d <= tol) return {};

	auto add = [&](const Point<U> &h, const Circle<U> &c) {
		for (const auto &t : tangent_points(h, c)) {
			Point<U> dir = t - h;
			if (sgn(square(dir)) == 0) dir = rotate_90(t - c.p);
			res.push_back(Line<U>(t, t + dir));
		}
	};

	if (std::abs(r1 - r2) <= tol) {
		Point<U> dir = (b - a) / d, n = rotate_90(dir);
		if (sgn(r1) != 0) {
			res.push_back(Line<U>(a + n * r1, b + n * r1));
			res.push_back(Line<U>(a - n * r1, b - n * r1));
		}
	} else if (d >= std::abs(r1 - r2) - tol) {
		Point<U> h = (b * r1 - a * r2) / (r1 - r2);
		add(h, Circle<U>(a, r1));
	}

	if (d >= r1 + r2 - tol) {
		Point<U> h = (b * r1 + a * r2) / (r1 + r2);
		add(h, Circle<U>(a, r1));
	}
	return res;
}

// Circumcenter of three points.  Returns the default Point (origin) when the
// points are collinear or otherwise degenerate; use circumcircle() when you
// need an explicit validity signal.
template <class T>
Point<calc_t<T>>
circumcenter(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
	using U = calc_t<T>;
	Point<U> x(a), y(b), z(c);
	U d = U(2) * cross(x, y) + U(2) * cross(y, z) + U(2) * cross(z, x);
	if (sgn(d) == 0) return {};
	U x2 = square(x), y2 = square(y), z2 = square(z);
	U cx = (x2 * (y.y - z.y) + y2 * (z.y - x.y) + z2 * (x.y - y.y)) / d;
	U cy = (x2 * (z.x - y.x) + y2 * (x.x - z.x) + z2 * (y.x - x.x)) / d;
	return Point<U>(cx, cy);
}

// Circumcircle of three points.  If the points are collinear, returns a
// circle with radius -1 as an invalid sentinel.
template <class T>
Circle<calc_t<T>>
circumcircle(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
	using U = calc_t<T>;
	if (sgn(cross(b - a, c - a)) == 0) return Circle<U>(Point<U>(), -1);
	Point<U> o = circumcenter(a, b, c);
	return Circle<U>(o, dist(o, a));
}

// Incenter of a nondegenerate triangle.  The weights are the side lengths
// opposite to a, b, and c respectively.  Input must not be collinear.
template <class T>
Point<calc_t<T>>
incenter(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
	using U = calc_t<T>;
	U x = dist(b, c), y = dist(c, a), z = dist(a, b);
	return Point<U>(
	    (x * a.x + y * b.x + z * c.x) / (x + y + z),
	    (x * a.y + y * b.y + z * c.y) / (x + y + z)
	);
}

// Minimum enclosing circle by randomized incremental (Welzl-style)
// construction.
// - The input is copied and shuffled with a fixed seed, so calls are
//   deterministic but the expected complexity is O(n).
// - Integral input is converted to calc_t (long double).
// - Empty input returns the default circle (origin and radius 0).
// - The returned circle covers all input points up to the configured eps;
//   points may lie on the boundary.
template <class T>
Circle<calc_t<T>> minimum_enclosing_circle(Polygon<T> points) {
	using U = calc_t<T>;
	std::mt19937 rng(712367821);
	std::shuffle(points.begin(), points.end(), rng);

	auto c1 = [&](const Point<U> &a) { return Circle<U>(a, 0); };
	auto c2 = [&](const Point<U> &a, const Point<U> &b) {
		return Circle<U>((a + b) / U(2), dist(a, b) / U(2));
	};
	auto c3 = [&](const Point<U> &a, const Point<U> &b, const Point<U> &c) {
		Circle<U> cc = circumcircle(a, b, c);
		if (cc.r >= 0) return cc;
		U d1 = dist(a, b), d2 = dist(b, c), d3 = dist(c, a);
		if (d1 >= d2 && d1 >= d3) return c2(a, b);
		if (d2 >= d1 && d2 >= d3) return c2(b, c);
		return c2(c, a);
	};

	Circle<U> c;
	bool ok = false;
	for (int i = 0; i < int(points.size()); i++) {
		Point<U> p(points[i]);
		if (ok && inside_circle(p, c)) continue;
		c = c1(p), ok = true;
		for (int j = 0; j < i; j++) {
			Point<U> q(points[j]);
			if (inside_circle(q, c)) continue;
			c = c2(p, q);
			for (int k = 0; k < j; k++) {
				Point<U> r(points[k]);
				if (inside_circle(r, c)) continue;
				c = c3(p, q, r);
			}
		}
	}
	return c;
}
