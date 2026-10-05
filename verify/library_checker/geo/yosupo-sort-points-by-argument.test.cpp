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
	std::sort(p.begin(), p.end(), polar_cmp<long long>);

	for (auto &[x, y] : p) std::cout << x << ' ' << y << '\n';
	return 0;
}
