// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/suffixarray

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/string/suffix_array.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	std::string s;
	optimizeIO(), std::cin >> s;

	SuffixArray SA(s);

	for (auto &x : SA.get_sa()) {
		std::cout << x << ' ';
	}
	return 0;
}
