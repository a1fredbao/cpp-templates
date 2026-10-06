// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cartesian_tree

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/cartesian.hpp"
#include <iostream>
#include <vector>

int main() {
	int n;
	optimizeIO(), std::cin >> n;

	std::vector<int> a(n), res(n);
	for (auto &x : a) std::cin >> x;

	auto [root, ch] = cartesian(a);
	for (int i = 0; i < n; i++) {
		if (ch[i].first != -1) res[ch[i].first] = i;
		if (ch[i].second != -1) res[ch[i].second] = i;
	}
	res[root] = root;
	for (int i = 0; i < n; i++) std::cout << res[i] << " \n"[i == n - 1];
	return 0;
}
