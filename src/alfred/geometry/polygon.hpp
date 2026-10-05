#pragma once

#include "line.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

// A polygon is a vertex sequence.  Edges are (p[i], p[(i+1)%n]); the polygon
// may be open in the container, even though it is geometrically closed.
// Functions that need an orientation document it explicitly.
template <class T>
using Polygon = std::vector<Point<T>>;

// Shoelace sum, i.e. twice the signed area.  Counterclockwise input is
// positive and clockwise input is negative.  This value is exact for
// integral coordinates and is often more convenient than actual area.
template <class T>
scalar_t<T> area(const Polygon<T> &p) {
	using R = scalar_t<T>;
	R s = 0;
	for (int i = 0, n = p.size(); i < n; i++) {
		s += cross(p[i], p[(i + 1) % n]);
	}
	return s;
}

// Centroid of a simple polygon.  The formula is signed and therefore works
// for clockwise and counterclockwise input.  If the signed area is zero,
// the function falls back to the arithmetic mean of the vertices.
// The result is floating-point when T is integral.
template <class T>
Point<calc_t<T>> centroid(const Polygon<T> &p) {
	using U = calc_t<T>;
	if (p.empty()) return {};
	using R = scalar_t<T>;
	R sw = 0;
	U x = 0, y = 0;
	for (int i = 0, n = p.size(); i < n; i++) {
		const Point<T> &a = p[i], &b = p[(i + 1) % n];
		R c = cross(a, b);
		sw += c;
		x += (U(a.x) + U(b.x)) * U(c);
		y += (U(a.y) + U(b.y)) * U(c);
	}
	if (sgn(sw) == 0) {
		U sx = 0, sy = 0;
		for (const auto &q : p) sx += q.x, sy += q.y;
		return Point<U>(sx / p.size(), sy / p.size());
	}
	U de = U(sw) * U(3);
	return Point<U>(x / de, y / de);
}

// Point classification by winding number.
// Return value: 0 outside, 1 strictly inside, 2 on an edge or vertex.
// The boundary check is performed first, so boundary points never become
// ambiguous.  The polygon is assumed to be simple.
// Complexity: O(n).
template <class T>
int point_in_polygon(const Point<T> &p, const Polygon<T> &poly) {
	int n = poly.size();
	for (int i = 0; i < n; i++) {
		if (point_on_segment(p, Line<T>(poly[i], poly[(i + 1) % n]))) {
			return 2;
		}
	}

	int wn = 0;
	for (int i = 0; i < n; i++) {
		Point<T> a = poly[i], b = poly[(i + 1) % n];
		if (cmp(a.y, p.y) <= 0) {
			if (cmp(b.y, p.y) > 0 && orient(a, b, p) > 0) wn++;
		} else {
			if (cmp(b.y, p.y) <= 0 && orient(a, b, p) < 0) wn--;
		}
	}
	return wn != 0 ? 1 : 0;
}

// Tests whether the closed segment l lies completely inside or on the
// boundary of poly.  The implementation cuts the segment at every
// intersection with a polygon edge and checks the midpoint of each resulting
// interval.  This handles concave polygons and boundary-touching segments,
// including segments passing through a vertex.
//
// Complexity: O(n^2) in the current form because each midpoint is tested
// with point_in_polygon.  It is intended for occasional robust checks, not
// as an inner loop of a high-performance algorithm.
template <class T>
bool segment_in_polygon(const Line<T> &l, const Polygon<T> &poly) {
	using U = calc_t<T>;
	if (l.a == l.b) return point_in_polygon(l.a, poly) != 0;

	Point<U> a(l.a), b(l.b), d = b - a;
	auto par = [&](const Point<U> &p) {
		if (std::abs(d.x) >= std::abs(d.y)) return (p.x - a.x) / d.x;
		return (p.y - a.y) / d.y;
	};

	std::vector<U> cs{0, 1};
	Polygon<U> q(poly.begin(), poly.end());
	for (int i = 0, n = poly.size(); i < n; i++) {
		auto [type, p, r] =
		    segment_intersection(l, Line<T>(poly[i], poly[(i + 1) % n]));
		if (type == 0) continue;
		cs.push_back(par(p));
		if (type == 2) cs.push_back(par(r));
	}
	std::sort(cs.begin(), cs.end());
	cs.erase(
	    std::unique(
	        cs.begin(), cs.end(),
	        [](U x, U y) { return std::abs(x - y) <= eps<U> * U(16); }
	    ),
	    cs.end()
	);

	for (int i = 0; i + 1 < int(cs.size()); i++) {
		U t = (cs[i] + cs[i + 1]) / 2;
		Point<U> m = a + d * t;
		if (point_in_polygon(m, q) == 0) return false;
	}
	return true;
}

// Clip a polygon by the closed left half-plane of directed line.
// Points with orient(line.a, line.b, p) >= 0 are kept, so points exactly on
// the clipping line are retained.  The routine works for any simple polygon
// (Sutherland-Hodgman clipping by one line), but the output may contain
// redundant collinear vertices.  A degenerate clipping line returns {}.
// The result uses calc_t<T>, so an integral input produces floating-point
// vertices.
template <class T>
Polygon<calc_t<T>> cut_polygon(const Polygon<T> &poly, const Line<T> &line) {
	using U = calc_t<T>;
	Polygon<U> p(poly.begin(), poly.end());
	if (p.empty() || line.a == line.b) return {};
	Point<U> la(line.a), lb(line.b);

	Polygon<U> res;
	auto add = [&](const Point<U> &p) {
		if (res.empty() || res.back() != p) res.push_back(p);
	};
	for (int i = 0, n = p.size(); i < n; i++) {
		Point<U> cur = p[i], nxt = p[(i + 1) % n];
		int sc = orient(la, lb, cur), sn = orient(la, lb, nxt);
		if (sc >= 0) {
			if (sn >= 0) {
				add(cur);
			} else {
				add(line_intersection(Line<U>(cur, nxt), Line<U>(la, lb)));
			}
		} else if (sn >= 0) {
			add(line_intersection(Line<U>(cur, nxt), Line<U>(la, lb)));
		}
	}
	if (!res.empty() && res.front() == res.back()) res.pop_back();
	return res;
}
