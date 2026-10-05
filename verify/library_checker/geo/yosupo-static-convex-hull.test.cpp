// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_convex_hull

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/geometry/convex_hull.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	int T;
	optimizeIO(), std::cin >> T;
	while (T--) {
		int n;
		std::cin >> n;
		std::vector<Point<long long>> p(n);
		for (auto &[x, y] : p) std::cin >> x >> y;
		auto hull = convex_hull(p);
		std::cout << hull.size() << '\n';
		for (auto &[x, y] : hull) std::cout << x << ' ' << y << '\n';
	}
	return 0;
}
