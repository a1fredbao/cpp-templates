#pragma once

#include "modint.hpp"
#include "prime.hpp"

#include <cmath>
#include <vector>

// Precompute powers of a fixed nonzero base. Prime modulus supports index
// reduction by mod - 1; otherwise require index < mod.
template <int base, class mint>
struct SOLPower {
	static constexpr bool P = is_prime(mint::mod());

	int sq;
	std::vector<mint> p1, ps;

	SOLPower(void) : sq(int(std::sqrt(mint::mod()))) {
		p1.assign(sq + 1, 1);
		for (int i = 1; i <= sq; i++) p1[i] = p1[i - 1] * base;
		ps = {1, p1.back()};
		for (int i = 2 * sq; i <= int(mint::mod()); i += sq) {
			ps.push_back(ps.back() * ps[1]);
		}
	}
	inline mint power(long long n) {
		if (P && n >= int(mint::mod())) n %= int(mint::mod()) - 1;
		return ps[n / sq] * p1[n % sq];
	}
};
