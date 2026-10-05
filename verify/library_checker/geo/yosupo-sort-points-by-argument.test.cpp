// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sort_points_by_argument

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/geometry/point.hpp"
#include <algorithm>
#include <iostream>

int main(int argc, char const *argv[]) {
	int n;
	optimizeIO(), std::cin >> n;
	std::vector<Point<long long>> p(n);
	for (auto &[x, y] : p) std::cin >> x >> y;

	// Sort by `atan2(y, x)` in (-pi, pi], i.e. counterclockwise from the
	// negative x-axis.
	auto half = [](const Point<long long> &a) {
		if (a.y < 0) return -1;
		if (a.y == 0 && a.x >= 0) return 0;
		return 1;
	};
	std::sort(
	    p.begin(), p.end(),
	    [&](const Point<long long> &a, const Point<long long> &b) {
		    int ha = half(a), hb = half(b);
		    if (ha != hb) return ha < hb;
		    return cross(a, b) > 0;
	    }
	);

	for (auto &[x, y] : p) std::cout << x << ' ' << y << '\n';
	return 0;
}
