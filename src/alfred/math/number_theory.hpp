#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
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

// Requires: mint::mod() is an odd prime.
template <class mint>
mint mod_sqrt(mint a) {
	using u64 = uint64_t;
	uint32_t p = mint::mod();
	if (a == 0) return 0;
	if (a.pow((p - 1) / 2) != 1) return -1;
	u64 q = p - 1, s = 0;
	for (; ~q & 1; q >>= 1) s++;
	mint z = 2;
	for (; z.pow((p - 1) / 2) == 1;) z += 1;
	mint c = z.pow(q), x = a.pow((q + 1) / 2), t = a.pow(q);
	for (u64 m = s; t != 1;) {
		u64 i = 1;
		mint u = t * t;
		for (; u != 1; u *= u) i++;
		mint b = c.pow(u64(1) << (m - i - 1));
		x *= b, c = b * b, t *= c, m = i;
	}
	return std::min(x.val(), p - x.val());
}
