#pragma once

#include "point.hpp"

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

// Directed line through a and b.  The same type also represents a segment or
// a ray; the interpretation is chosen by the function being called.
template <class T>
struct Line {
	Point<T> a{}, b{};
	Line() = default;
	Line(Point<T> a_, Point<T> b_) : a(a_), b(b_) {}
};

template <class T>
using Segment = Line<T>;
template <class T>
using Ray = Line<T>;

// +1 if c is to the left of directed line a -> b, -1 if it is to the right,
// and 0 if the three points are collinear (within eps for floating T).
template <class T>
constexpr int orient(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
	return sgn(cross(b - a, c - a));
}

// Strict left-side test: boundary points are not considered to be on the left.
template <class T>
bool to_left(const Point<T> &p, const Line<T> &l) {
	return orient(l.a, l.b, p) > 0;
}

// Readable alias for to_left.
template <class T>
bool point_on_line_left(const Point<T> &p, const Line<T> &l) {
	return to_left(p, l);
}

// True when p is on the infinite line through l.a and l.b.
// For a zero-length line this is true only because the line is degenerate;
// use point_on_segment for the intended containment test.
template <class T>
bool point_on_line(const Point<T> &p, const Line<T> &l) {
	return orient(l.a, l.b, p) == 0;
}

// Inclusive segment containment test.  Endpoints count as contained.
// For a zero-length segment l only l.a is contained.
template <class T>
bool point_on_segment(const Point<T> &p, const Line<T> &l) {
	if (!point_on_line(p, l)) return false;
	return cmp(std::min(l.a.x, l.b.x), p.x) <= 0 &&
	       cmp(p.x, std::max(l.a.x, l.b.x)) <= 0 &&
	       cmp(std::min(l.a.y, l.b.y), p.y) <= 0 &&
	       cmp(p.y, std::max(l.a.y, l.b.y)) <= 0;
}

// Parallel direction test.  Zero-length lines are reported as parallel to
// every line because their direction vector is zero.
template <class T>
bool parallel(const Line<T> &l1, const Line<T> &l2) {
	return orient(Point<T>(), l1.b - l1.a, l2.b - l2.a) == 0;
}

// Intersection of the two infinite lines.  Parallel or coincident lines
// return the default Point, which is also the origin; check parallel() first
// if the origin is a valid intersection in your problem.
template <class T>
Point<calc_t<T>> line_intersection(const Line<T> &l1, const Line<T> &l2) {
	using U = calc_t<T>;
	Point<U> a(l1.a), b(l1.b), c(l2.a), d(l2.b);
	U de = cross(b - a, d - c);
	if (sgn(de) == 0) return {};
	return a + (b - a) * (cross(c - a, d - c) / de);
}

// Orthogonal projection of p onto the infinite line l.
// A zero-length line returns l.a.
template <class T>
Point<calc_t<T>> projection(const Point<T> &p, const Line<T> &l) {
	using U = calc_t<T>;
	Point<U> q(p), a(l.a), b(l.b), v = b - a;
	if (sgn(square(v)) == 0) return a;
	return a + v * (dot(q - a, v) / dot(v, v));
}

// Reflection (mirror image) of p across the infinite line l.
// A zero-length line reflects through the point l.a.
template <class T>
Point<calc_t<T>> reflection(const Point<T> &p, const Line<T> &l) {
	using U = calc_t<T>;
	Point<U> q(p);
	return projection(p, l) * U(2) - q;
}

// Argument order mirror(l, p) is convenient when processing a line first.
template <class T>
Point<calc_t<T>> mirror(const Line<T> &l, const Point<T> &p) {
	return reflection(p, l);
}

// Distance from p to the infinite line l.  A zero-length line is treated as
// a point, returning dist(p, l.a).
template <class T>
calc_t<T> dist_point_line(const Point<T> &p, const Line<T> &l) {
	using U = calc_t<T>;
	Point<U> q(p), a(l.a), b(l.b), v = b - a;
	if (sgn(square(v)) == 0) return dist(q, a);
	return std::abs(cross(v, q - a)) / length(v);
}

// Distance from p to the closed segment l.  If the projection falls outside
// the segment, the distance to an endpoint is returned.
template <class T>
calc_t<T> dist_point_segment(const Point<T> &p, const Line<T> &l) {
	using U = calc_t<T>;
	Point<U> q(p), a(l.a), b(l.b), v = b - a;
	if (sgn(square(v)) == 0) return dist(q, a);
	if (sgn(dot(q - a, v)) < 0) return dist(q, a);
	if (sgn(dot(q - b, -v)) < 0) return dist(q, b);
	return dist_point_line(p, l);
}

// Classification of the relation between two closed segments.
// Return value:
//   0: disjoint
//   1: proper crossing (interior of both segments)
//   2: collinear overlap of positive length
//   3: touch at one or more endpoints (possibly a single point)
// The test is exact for integral coordinates.  Floating inputs use the
// configured eps through orient().
template <class T>
int segment_intersection_check(const Line<T> &l1, const Line<T> &l2) {
	int o1 = orient(l1.a, l1.b, l2.a);
	int o2 = orient(l1.a, l1.b, l2.b);
	int o3 = orient(l2.a, l2.b, l1.a);
	int o4 = orient(l2.a, l2.b, l1.b);

	if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) {
		T lx = std::max(std::min(l1.a.x, l1.b.x), std::min(l2.a.x, l2.b.x));
		T hx = std::min(std::max(l1.a.x, l1.b.x), std::max(l2.a.x, l2.b.x));
		T ly = std::max(std::min(l1.a.y, l1.b.y), std::min(l2.a.y, l2.b.y));
		T hy = std::min(std::max(l1.a.y, l1.b.y), std::max(l2.a.y, l2.b.y));
		if (cmp(lx, hx) > 0 || cmp(ly, hy) > 0) return 0;
		return cmp(lx, hx) == 0 && cmp(ly, hy) == 0 ? 3 : 2;
	}
	if (o1 * o2 < 0 && o3 * o4 < 0) return 1;
	if (o1 == 0 && point_on_segment(l2.a, l1)) return 3;
	if (o2 == 0 && point_on_segment(l2.b, l1)) return 3;
	if (o3 == 0 && point_on_segment(l1.a, l2)) return 3;
	if (o4 == 0 && point_on_segment(l1.b, l2)) return 3;
	return 0;
}

// Detailed segment intersection result.
// The tuple is {type, first, second}:
//   type 0: first = second = default Point, no intersection;
//   type 1: first = second = the proper crossing point;
//   type 2: first/second are the endpoints of the overlap segment, ordered
//           lexicographically (not necessarily in the input direction);
//   type 3: first = second = the contact point.
// For overlapping collinear segments, the two returned points are endpoints
// of the overlap; if the overlap is one point, type 3 is returned.
template <class T>
std::tuple<int, Point<calc_t<T>>, Point<calc_t<T>>>
segment_intersection(const Line<T> &l1, const Line<T> &l2) {
	using U = calc_t<T>;
	int type = segment_intersection_check(l1, l2);
	if (type == 0) return {0, Point<U>(), Point<U>()};
	if (type == 1) {
		Point<U> p = line_intersection(l1, l2);
		return {1, p, p};
	}
	if (type == 3) {
		for (const auto *q : {&l1.a, &l1.b, &l2.a, &l2.b}) {
			if (point_on_segment(*q, l1) && point_on_segment(*q, l2)) {
				return {3, Point<U>(*q), Point<U>(*q)};
			}
		}
		Point<U> x = line_intersection(l1, l2);
		return {3, x, x};
	}

	std::vector<Point<T>> cand{l1.a, l1.b, l2.a, l2.b};
	std::sort(cand.begin(), cand.end());
	cand.erase(std::unique(cand.begin(), cand.end()), cand.end());
	std::vector<Point<T>> ov;
	for (const auto &q : cand) {
		if (point_on_segment(q, l1) && point_on_segment(q, l2)) {
			ov.push_back(q);
		}
	}
	if (ov.size() == 1) {
		return {3, Point<U>(ov[0]), Point<U>(ov[0])};
	}
	return {2, Point<U>(ov.front()), Point<U>(ov.back())};
}

// Minimum distance between two closed segments.  Intersecting or touching
// segments return 0.  Otherwise it is the minimum of the four
// endpoint-to-other-segment distances.
template <class T>
calc_t<T> dist_segment_segment(const Line<T> &l1, const Line<T> &l2) {
	if (segment_intersection_check(l1, l2) != 0) return 0;
	return std::min(
	    {dist_point_segment(l1.a, l2), dist_point_segment(l1.b, l2),
		 dist_point_segment(l2.a, l1), dist_point_segment(l2.b, l1)}
	);
}
