#ifndef AFMT_GCD
#define AFMT_GCD

#include <array>
#include <bitset>
#include <cmath>
#if __cplusplus >= 201103L
#include <utility>
#else
#include <algorithm>
#endif
#include <vector>

template <const int V>
class ValueRangeGCD {
private:
	std::bitset<V + 1> vis;
	std::vector<int> primes;
	static constexpr int B = std::sqrt(V) + 2;
	std::array<std::array<int, B>, B> tab;
	std::array<std::array<int, 3>, V + 1> fac;

public:
	ValueRangeGCD(void) {
		for (int i = 0; i < B; i++) {
			tab[0][i] = tab[i][0] = i;
		}
		for (int i = 1; i < B; i++) {
			for (int j = 1; j <= i; j++) {
				tab[i][j] = tab[j][i] = tab[j][i % j];
			}
		}
		fac[1][0] = fac[1][1] = fac[1][2] = 1;
		for (int i = 2; i <= V; i++) {
			if (!vis.test(i)) {
				fac[i][0] = fac[i][1] = 1;
				fac[i][2] = i, primes.push_back(i);
			}
			for (auto &p : primes) {
				if (i * p > V) break;
				const int j = i * p;
				fac[j] = fac[i], vis[j] = 1, fac[j][0] *= p;
				if (fac[j][0] > fac[j][1]) {
					std::swap(fac[j][0], fac[j][1]);
				}
				if (fac[j][1] > fac[j][2]) {
					std::swap(fac[j][1], fac[j][2]);
				}
				if (i % p == 0) break;
			}
		}
	}
	inline int gcd(int x, int y) {
		int ans = 1, cur;
		for (int i = 0; i < 3; i++) {
			if (fac[x][i] >= B) {
				cur = y % fac[x][i] ? 1 : fac[x][i];
			} else {
				cur = tab[fac[x][i]][y % fac[x][i]];
			}
			y /= cur, ans *= cur;
		}
		return ans;
	}
};

template <class T>
inline T gcd(T a, T b) {
	return b == 0 ? a : gcd(b, a % b);
}

#endif // AFMT_GCD
