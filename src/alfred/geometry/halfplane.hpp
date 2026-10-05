#pragma once

#include "polygon.hpp"

#include <algorithm>
#include <deque>
#include <vector>

// A half-plane is the open/closed region to the left of the directed line
// a -> b.  The boundary itself is included in the intersection routine.
// The direction is therefore significant: reversing a and b keeps the same
// geometric line but returns the complementary half-plane.
template <class T>
struct Halfplane : Line<T> {
	using Line<T>::Line;
};

// Intersection of a set of half-planes, using the deque angular-sweep
// algorithm.
// - Input order is arbitrary.
// - The result is a counterclockwise convex polygon; boundary points are
//   included.
// - An empty result means that the intersection is empty, a single point, a
//   segment, or unbounded.  This routine is intended for bounded polygons.
// - Parallel half-planes in the same direction keep the most restrictive
//   one; opposite parallel directions immediately make the result empty.
// - The computation is performed in calc_t<T>, so integral input returns
//   long double vertices.
// Complexity: O(n log n) for sorting plus O(n) deque work.
template <class T>
Polygon<calc_t<T>> halfplane_intersection(std::vector<Halfplane<T>> hs) {
	using U = calc_t<T>;
	std::vector<Halfplane<U>> ls;
	ls.reserve(hs.size());
	for (const auto &h : hs) {
		ls.push_back({Point<U>(h.a), Point<U>(h.b)});
	}
	std::sort(ls.begin(), ls.end(), [](const auto &x, const auto &y) {
		return polar_cmp(x.b - x.a, y.b - y.a);
	});

	std::deque<Halfplane<U>> q;
	std::deque<Point<U>> ps;
	for (const auto &l : ls) {
		while (!ps.empty() && !to_left(ps.back(), l)) {
			ps.pop_back();
			q.pop_back();
		}
		while (!ps.empty() && !to_left(ps.front(), l)) {
			ps.pop_front();
			q.pop_front();
		}
		if (!q.empty()) {
			Point<U> d1 = q.back().b - q.back().a;
			Point<U> d2 = l.b - l.a;
			if (sgn(cross(d1, d2)) == 0) {
				if (sgn(dot(d1, d2)) > 0) {
					if (!to_left(q.back().a, l)) q.back() = l;
					continue;
				}
				return {};
			}
			ps.push_back(line_intersection(q.back(), l));
		}
		q.push_back(l);
	}

	while (q.size() > 1 && !to_left(ps.back(), q.front())) {
		ps.pop_back();
		q.pop_back();
	}
	while (q.size() > 1 && !to_left(ps.front(), q.back())) {
		ps.pop_front();
		q.pop_front();
	}
	if (q.size() <= 2) return {};
	ps.push_back(line_intersection(q.front(), q.back()));
	return {ps.begin(), ps.end()};
}
