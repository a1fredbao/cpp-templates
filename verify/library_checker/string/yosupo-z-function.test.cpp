// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/zalgorithm

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/string/z_function.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
	std::string s;
	optimizeIO(), std::cin >> s;

	for (auto &x : z_function(s)) {
		std::cout << x << ' ';
	}
	return 0;
}
