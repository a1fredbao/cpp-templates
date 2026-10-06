// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/double_ended_priority_queue

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/priority_set.hpp"
#include <functional>
#include <iostream>

int main() {
	int n, q, opt, x;
	optimizeIO(), std::cin >> n >> q;

	PrioritySet<int, std::less<int>> lo;
	PrioritySet<int, std::greater<int>> hi;
	for (int i = 0; i < n; i++) {
		std::cin >> x, lo.insert(x), hi.insert(x);
	}
	while (q--) {
		std::cin >> opt;
		if (opt == 0) {
			std::cin >> x, lo.insert(x), hi.insert(x);
		} else if (opt == 1) {
			x = hi.top(), std::cout << x << '\n';
			lo.erase(x), hi.erase(x);
		} else {
			x = lo.top(), std::cout << x << '\n';
			lo.erase(x), hi.erase(x);
		}
	}
	return 0;
}
