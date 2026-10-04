// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/two_sat

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/graph/two_sat.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	std::string s;
	int n, m, x, y, v;
	optimizeIO(), std::cin >> s >> s >> n >> m;

	TwoSAT TS(n);

	while (m--) {
		std::cin >> x >> y >> v;
		bool v1 = true, v2 = true;
		if (x < 0) v1 = false, x = -x;
		if (y < 0) v2 = false, y = -y;
		TS.add(x - 1, v1, y - 1, v2);
	}

	TS.init();

	if (TS.has_solution()) {
		std::cout << "s SATISFIABLE\nv ";
		std::vector<int> res = TS.solve();
		for (int i = 0; i < n; i++) {
			if (res[i] == 0) {
				std::cout << -(i + 1) << ' ';
			} else {
				std::cout << i + 1 << ' ';
			}
		}
		std::cout << "0\n";
	} else {
		std::cout << "s UNSATISFIABLE\n";
	}

	return 0;
}
