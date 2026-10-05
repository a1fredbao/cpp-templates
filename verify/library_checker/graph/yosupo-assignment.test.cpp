// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/assignment

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/graph/min_cost_flow.hpp"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[]) {
	int n;
	optimizeIO(), std::cin >> n;

	const int source = 0, sink = 2 * n + 1;
	MCMF_SPFA<int, long long> flow(2 * n + 2);
	for (int i = 0; i < n; i++) {
		flow.add(source, 1 + i, 1, 0);
		flow.add(1 + n + i, sink, 1, 0);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			long long a;
			std::cin >> a;
			flow.add(1 + i, 1 + n + j, 1, a);
		}
	}

	auto [matching, cost] = flow.maxflow(source, sink);

	std::vector<int> p(n);
	for (auto &e : flow.edges()) {
		if (e.flow == 1 && 1 <= e.from && e.from <= n && 1 + n <= e.to &&
		    e.to <= 2 * n) {
			p[e.from - 1] = e.to - n - 1;
		}
	}

	std::cout << cost << '\n';
	for (int i = 0; i < n; i++) {
		std::cout << p[i] << " \n"[i + 1 == n];
	}

	return 0;
}
