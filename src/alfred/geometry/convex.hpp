#pragma once

#include "polygon.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <tuple>
#include <vector>

// Andrew monotone-chain convex hull.
// - Duplicate input points are removed.
// - The returned vertices are counterclockwise and start at the
//   lexicographically smallest unique vertex.
// - include_collinear == false removes points lying on straight boundary
//   edges.  include_collinear == true keeps them, and for all-collinear input
//   returns the unique points in sorted order.
// - Inputs with at most two points are returned directly.
// Complexity: O(n log n) sorting plus O(n) construction.
template <class T>
Polygon<T>
convex_hull(const Polygon<T> &points, bool include_collinear = false) {
	Polygon<T> p = points;
	std::sort(p.begin(), p.end());
	p.erase(std::unique(p.begin(), p.end()), p.end());
	if (p.size() <= 1) return p;

	bool col = true;
	for (int i = 2; i < int(p.size()); i++) {
		if (orient(p[0], p[1], p[i]) != 0) {
			col = false;
			break;
		}
	}
	if (col) {
		if (include_collinear) return p;
		return {p.front(), p.back()};
	}

	auto mk = [&](const Polygon<T> &q) {
		Polygon<T> h;
		for (const Point<T> &x : q) {
			while (h.size() > 1) {
				auto c = cross(h.back() - h[h.size() - 2], x - h.back());
				if (include_collinear ? c < 0 : c <= 0) {
					h.pop_back();
				} else {
					break;
				}
			}
			h.push_back(x);
		}
		return h;
	};

	Polygon<T> lo = mk(p);
	Polygon<T> rv = p;
	std::reverse(rv.begin(), rv.end());
	Polygon<T> hi = mk(rv);
	lo.pop_back();
	hi.pop_back();
	lo.insert(lo.end(), hi.begin(), hi.end());
	if (include_collinear) {
		lo.erase(std::unique(lo.begin(), lo.end()), lo.end());
	}
	return lo;
}

// Convexity test in the polygon's cyclic order.
// Collinear consecutive triples are allowed.  All nonzero turns must have
// the same sign.  The function does not detect self-intersection.
template <class T>
bool is_convex(const Polygon<T> &p) {
	int n = p.size();
	if (n < 3) return false;
	int sg = 0;
	for (int i = 0; i < n; i++) {
		int c = orient(p[i], p[(i + 1) % n], p[(i + 2) % n]);
		if (c < 0) sg |= 1;
		if (c > 0) sg |= 2;
	}
	return sg != 3;
}

// Point-in-convex-polygon by sector binary search.
// Preconditions: poly is convex and counterclockwise, with cyclic vertex
// order.  Returns 0 outside, 1 strictly inside, and 2 on the boundary.
// The cases n <= 2 degenerate to point/segment containment.
// Complexity: O(log n).
template <class T>
int point_in_convex(const Point<T> &p, const Polygon<T> &poly) {
	int n = poly.size();
	if (n == 0) return 0;
	if (n == 1) return p == poly[0] ? 2 : 0;
	if (n == 2) {
		return point_on_segment(p, Line<T>(poly[0], poly[1])) ? 2 : 0;
	}

	const Point<T> &a = poly[0];
	int c1 = orient(a, poly[1], p);
	int c2 = orient(a, poly[n - 1], p);
	if (c1 == 0) return point_on_segment(p, Line<T>(a, poly[1])) ? 2 : 0;
	if (c2 == 0) return point_on_segment(p, Line<T>(a, poly[n - 1])) ? 2 : 0;
	if (c1 < 0 || c2 > 0) return 0;

	int lo = 1, hi = n - 1;
	while (hi - lo > 1) {
		int mid = (lo + hi) / 2;
		if (orient(a, poly[mid], p) >= 0) {
			lo = mid;
		} else {
			hi = mid;
		}
	}
	int c = orient(poly[lo], poly[lo + 1], p);
	if (c < 0) return 0;
	return c == 0 ? 2 : 1;
}

// Diameter of a counterclockwise convex polygon by rotating calipers.
// Returns {actual Euclidean distance, p, q}, where p/q is a farthest pair.
// Ties are arbitrary.  Inputs with fewer than three vertices degenerate
// naturally.  Complexity: O(n).
template <class T>
std::tuple<calc_t<T>, Point<T>, Point<T>> convex_diameter(const Polygon<T> &p) {
	using R = scalar_t<T>;
	int n = p.size();
	if (n == 0) return {0, Point<T>(), Point<T>()};
	if (n == 1) return {0, p[0], p[0]};
	if (n == 2) return {dist(p[0], p[1]), p[0], p[1]};

	auto val = [&](int i, int j) {
		R c = cross(p[(i + 1) % n] - p[i], p[j] - p[i]);
		return c < 0 ? -c : c;
	};
	int j = 1, ai = 0, bi = 1;
	R best = -1;
	for (int i = 0; i < n; i++) {
		while (val(i, (j + 1) % n) > val(i, j)) j = (j + 1) % n;
		for (int x : {i, (i + 1) % n}) {
			R d = square(p[x] - p[j]);
			if (d > best) best = d, ai = x, bi = j;
		}
	}
	return {dist(p[ai], p[bi]), p[ai], p[bi]};
}

// Maximum-area triangle with vertices chosen from a convex polygon.
// Input must be counterclockwise.  The returned value is the actual area
// (not twice the area).  Complexity: O(n^2).
template <class T>
calc_t<T> max_triangle_area(const Polygon<T> &convex) {
	using U = calc_t<T>;
	using R = scalar_t<T>;
	int n = convex.size();
	if (n < 3) return 0;
	Polygon<T> p = convex;
	p.insert(p.end(), convex.begin(), convex.end() - 1);

	R best = 0;
	auto area2 = [&](int i, int j, int k) {
		R c = cross(p[i], p[j], p[k]);
		return c < 0 ? -c : c;
	};
	for (int i = 0; i < n; i++) {
		int k = i + 2;
		for (int j = i + 1; j < i + n - 1; j++) {
			if (k <= j) k = j + 1;
			while (k + 1 < i + n && area2(i, j, k + 1) >= area2(i, j, k)) {
				k++;
			}
			R cur = area2(i, j, k);
			if (cur > best) best = cur;
		}
	}
	return U(best) / 2;
}

// Minimum-area and minimum-perimeter enclosing rectangle results.
// The two rectangles are computed independently; they need not be equal.
// For counterclockwise input, both output rectangles are counterclockwise.
template <class T>
struct BoundingRectangleResult {
	calc_t<T> area = 0, perimeter = 0;
	Polygon<calc_t<T>> area_rect, perimeter_rect;
};

// Minimum-area and minimum-perimeter enclosing rectangles by rotating
// calipers.  The input must be a nondegenerate counterclockwise convex
// polygon.  If n < 3, all fields remain zero/empty.  area and perimeter are
// true values, not doubled.  Complexity: O(n).
template <class T>
BoundingRectangleResult<T> min_bounding_rectangle(const Polygon<T> &convex) {
	using U = calc_t<T>;
	using R = scalar_t<T>;
	int n = convex.size();
	BoundingRectangleResult<T> res;
	if (n < 3) return res;

	Point<T> e0 = convex[1] - convex[0];
	auto h0 = [&](int x) {
		R c = cross(e0, convex[x] - convex[0]);
		return c < 0 ? -c : c;
	};
	auto p0 = [&](int x) { return dot(e0, convex[x] - convex[0]); };
	int j = 1, k = 1, l = 1;
	for (int x = 1; x < n; x++) {
		if (h0(x) > h0(j)) j = x;
		if (p0(x) > p0(k)) k = x;
		if (p0(x) < p0(l)) l = x;
	}
	U ba = -1, bp = -1;
	for (int i = 0; i < n; i++) {
		int ni = (i + 1) % n;
		Point<T> e = convex[ni] - convex[i];
		auto h = [&](int x) {
			R c = cross(e, convex[x] - convex[i]);
			return c < 0 ? -c : c;
		};
		auto pr = [&](int x) { return dot(e, convex[x] - convex[i]); };

		for (int step = 0; step + 1 < n; step++) {
			int nxt = (j + 1) % n;
			if (h(nxt) <= h(j)) break;
			j = nxt;
		}
		for (int step = 0; step + 1 < n; step++) {
			int nxt = (k + 1) % n;
			if (pr(nxt) <= pr(k)) break;
			k = nxt;
		}
		for (int step = 0; step + 1 < n; step++) {
			int nxt = (l + 1) % n;
			if (pr(nxt) >= pr(l)) break;
			l = nxt;
		}

		R l2 = dot(e, e);
		U ln = std::sqrt(U(l2));
		U ht = U(h(j)) / ln;
		U w = (U(pr(k)) - U(pr(l))) / ln;
		U ar = w * ht, pe = U(2) * (w + ht);
		Point<U> o = Point<U>(convex[i]) + Point<U>(e) * (U(pr(l)) / U(l2));
		Point<U> u = Point<U>(e) / ln;
		Point<U> v = rotate_90(u);
		Polygon<U> rc{o, o + u * w, o + u * w + v * ht, o + v * ht};
		if (ba < 0 || ar < ba) {
			ba = ar, res.area = ar, res.area_rect = rc;
		}
		if (bp < 0 || pe < bp) {
			bp = pe, res.perimeter = pe, res.perimeter_rect = rc;
		}
	}
	return res;
}

// Minkowski sum of counterclockwise convex polygons.
// The O(n+m) edge-vector merge is used when both inputs have at least three
// vertices; small inputs use a safe pairwise-sum fallback with
// O(nm log(nm)) work.  Collinear edges are merged.  The result is
// counterclockwise and starts at the bottommost (then leftmost) vertex.
// Empty input returns an empty polygon.
template <class T>
Polygon<T> minkowski_sum(const Polygon<T> &a, const Polygon<T> &b) {
	using R = scalar_t<T>;
	int n = a.size(), m = b.size();
	if (n == 0 || m == 0) return {};
	if (n <= 2 || m <= 2) {
		Polygon<T> sums;
		for (const auto &x : a) {
			for (const auto &y : b) sums.push_back(x + y);
		}
		return convex_hull(sums);
	}

	auto low = [&](const Polygon<T> &p) {
		int id = 0;
		for (int i = 1; i < int(p.size()); i++) {
			if (p[i].y < p[id].y || (p[i].y == p[id].y && p[i].x < p[id].x)) {
				id = i;
			}
		}
		return id;
	};
	int i = low(a), j = low(b);
	Point<T> cur = a[i] + b[j];
	Polygon<T> res{cur};
	int cnt = 0;
	while (cnt < n + m) {
		Point<T> e1 = a[(i + 1) % n] - a[i];
		Point<T> e2 = b[(j + 1) % m] - b[j];
		R c = cross(e1, e2);
		if (c > 0) {
			cur += e1, i = (i + 1) % n, cnt++;
		} else if (c < 0) {
			cur += e2, j = (j + 1) % m, cnt++;
		} else {
			cur += e1 + e2;
			i = (i + 1) % n, j = (j + 1) % m, cnt += 2;
		}
		if (res.back() != cur) res.push_back(cur);
	}
	if (res.size() > 1 && res.front() == res.back()) res.pop_back();
	return res;
}

// Minimum distance between counterclockwise convex polygons.
// The code forms the Minkowski difference P + (-Q).  If the origin lies
// inside or on that difference, the answer is 0.  Otherwise it is the
// minimum distance from the origin to an edge of the difference polygon.
template <class T>
calc_t<T> min_dist_convex(const Polygon<T> &a, const Polygon<T> &b) {
	using U = calc_t<T>;
	if (a.empty() || b.empty()) return 0;
	Polygon<T> ng;
	for (const auto &p : b) ng.push_back(-p);
	Polygon<T> d = minkowski_sum(a, ng);
	if (point_in_convex(Point<T>(), d) != 0) return 0;

	U ans = std::numeric_limits<U>::infinity();
	for (int i = 0, n = d.size(); i < n; i++) {
		ans = std::min(
		    ans, dist_point_segment(Point<T>(), Line<T>(d[i], d[(i + 1) % n]))
		);
	}
	return ans;
}
