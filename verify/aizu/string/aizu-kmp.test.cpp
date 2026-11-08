// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_14_B

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/string/kmp.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	std::string S, T;
	optimizeIO(), std::cin >> S >> T;
	for (auto &p : kmp(S, T)) {
		std::cout << p << '\n';
	}
	return 0;
}
