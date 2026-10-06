#pragma once

// Calculate $\sum_{i = 0}^{n - 1} \lfloor \frac{ai + b}{m} \rfloor$ in
// $O(\min\{a, m, n\})$
inline unsigned long long floor_sum_unsigned(
    unsigned long long a, unsigned long long b, unsigned long long m,
    unsigned long long n
) {
	if (a >= m || b >= m) {
		return (a / m) * (n * (n - 1) / 2) + b / m * n +
		       floor_sum_unsigned(a % m, b % m, m, n);
	}
	unsigned long long mx = (a * (n - 1) + b) / m;
	return mx ? (n - 1) * mx - floor_sum_unsigned(m, m - b - 1, a, mx) : 0;
}

// Calculate $\sum_{i = 0}^{n - 1} \lfloor \frac{ai + b}{m} \rfloor$ in
// $O(\min\{a, m, n\})$
inline long long floor_sum(long long a, long long b, long long m, long long n) {
	unsigned long long ans = 0;
	if (a < 0) {
		unsigned long long a2 = (a % m + m) % m;
		ans -= 1ull * n * (n - 1) / 2 * ((a2 - a) / m), a = a2;
	}
	if (b < 0) {
		unsigned long long b2 = (b % m + m) % m;
		ans -= 1ull * n * ((b2 - b) / m), b = b2;
	}
	return ans + floor_sum_unsigned(a, b, m, n);
}
