// competitive-verifier: PROBLEM
// https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0528

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/string/suffix_array.hpp"
#include <iostream>

std::string S, T;

inline void solve(void) {
	SuffixArray SA(S + '#' + T);
	int n = S.size(), m = T.size(), ans = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			ans = std::max(ans, SA.lcp(i, j + n + 1));
		}
	}
	std::cout << ans << '\n';
}

int main(int argc, char const *argv[]) {
	optimizeIO();
	while (std::cin >> S >> T) {
		solve();
	}
	return 0;
}
