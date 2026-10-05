// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_5_D

#include "../../../src/alfred/algorithm/pair_counting.hpp"
#include "../../../src/alfred/core/io.hpp"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[]) {
	int n;
	optimizeIO(), std::cin >> n;
	std::vector<int> a(n);
	for (auto &x : a) std::cin >> x;
	std::cout << count_pair(a, std::greater<>()) << '\n';
	return 0;
}
