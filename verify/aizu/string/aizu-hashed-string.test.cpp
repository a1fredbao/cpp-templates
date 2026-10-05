// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=0528

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/string/hash.hpp"
#include <iostream>

std::string S, T;

inline void solve(void) {
	HashedString H(S + T);
	int ans = 0, L, R, mid;
	int n = S.size(), m = T.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (S[i] != T[j]) continue;
			L = 0, R = std::min(n - i, m - j) - 1;
			if (R + 1 < ans) continue;
			while (L < R) {
				mid = (L + R + 1) >> 1;
				if (H.check_same(i, i + mid, j + n, j + n + mid)) {
					L = mid;
				} else {
					R = mid - 1;
				}
			}
			ans = std::max(ans, L + 1);
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
