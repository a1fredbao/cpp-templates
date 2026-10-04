#pragma once

#include "point.hpp"
#include <algorithm>
#include <vector>

// Monotone chain convex hull. O(N log N).
// By default `strict = true` removes collinear points on edges.
template <class T>
std::vector<Point<T>> convex_hull(std::vector<Point<T>> P, bool strict = true) {
	int n = P.size(), k = 0;
	if (n <= 1) return P;
	std::sort(P.begin(), P.end());

	std::vector<Point<T>> H(2 * n);
	auto check = [&](const Point<T> &a, const Point<T> &b, const Point<T> &c) {
		int s = sgn(cross(a, b, c));
		return strict ? (s <= 0) : (s < 0);
	};

	for (int i = 0; i < n; ++i) {
		while (k >= 2 && check(H[k - 2], H[k - 1], P[i])) k--;
		H[k++] = P[i];
	}
	for (int i = n - 2, t = k + 1; i >= 0; --i) {
		while (k >= t && check(H[k - 2], H[k - 1], P[i])) k--;
		H[k++] = P[i];
	}
	H.resize(k - 1);
	return H;
}
