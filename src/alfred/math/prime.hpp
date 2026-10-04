#pragma once

#include <utility>
#include <vector>

// O(sqrt(x)) to judge if x is a prime.
// It will be re-written in Miller-rabin sometime.
template <class T>
constexpr bool is_prime(T x) {
	if (x < 2) return false;
	for (T i = 2; i * i <= x; i++) {
		if (x % i == 0) return false;
	}
	return true;
}

// Given in integer n. Returns (primes, minp)
std::pair<std::vector<int>, std::vector<int>> euler_sieve(int n) {
	std::vector<int> primes, minp(n + 1);
	for (int i = 2; i <= n; i++) {
		if (minp[i] == 0) {
			minp[i] = i;
			primes.push_back(i);
		}
		for (auto &p : primes) {
			if (i * p > n) break;
			minp[i * p] = p;
			if (i % p == 0) break;
		}
	}
	return std::make_pair(primes, minp);
}

template <class T>
std::vector<std::pair<T, int>> factorize(T n) { // O(sqrt(n)) factorization.
	std::vector<std::pair<T, int>> vec;
	for (T i = 2; i * i <= n; i++) {
		int cnt = 0;
		if (n % i != 0) continue;
		while (n % i == 0) {
			n /= i, cnt++;
		}
		vec.push_back({i, cnt});
	}
	if (n != 1) {
		vec.push_back({n, 1});
	}
	return vec;
}

template <class T>
inline T phi(T n) {
	auto vec = factorize(n);
	for (auto &[p, cnt] : vec) {
		n /= p, n *= (p - 1);
	}
	return n;
}
