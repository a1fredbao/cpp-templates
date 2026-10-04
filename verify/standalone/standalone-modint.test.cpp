// competitive-verifier: STANDALONE

#include "../../src/alfred/math/modint.hpp"
#include <cassert>
#include <cstdint>
#include <random>

template <class mint>
void check_modint(uint32_t mod) {
	std::mt19937_64 rng(123456789);
	auto norm = [&](int64_t x) -> uint32_t {
		x %= int64_t(mod);
		if (x < 0) x += mod;
		return uint32_t(x);
	};
	for (int iteration = 0; iteration < 20000; iteration++) {
		int64_t a = int64_t(rng() % mod);
		int64_t b = int64_t(rng() % mod);
		mint x = a, y = b;
		assert(x.val() == norm(a));
		assert(y.val() == norm(b));
		assert((x + y).val() == norm(a + b));
		assert((x - y).val() == norm(a - b));
		assert((x * y).val() == uint32_t(__int128(a) * b % mod));
		if (x.val() != 0 && y.val() != 0) {
			assert(
			    (x / y).val() ==
			    uint32_t(uint64_t(x.val()) * y.inv().val() % mod)
			);
			assert(x.pow(-1) == x.inv());
		}
		assert(x.pow(0) == mint(1));
		assert(x.pow(1) == x);
		assert(x.pow(2) == x * x);
	}
}

int main() {
	check_modint<m998>(998244353);
	check_modint<m107>(1000000007);
	check_modint<ModInt<1004535809u>>(1004535809);

	DynamicModInt::set_mod(998244353);
	check_modint<dint>(998244353);
	DynamicModInt::set_mod(1000000007);
	check_modint<dint>(1000000007);
	return 0;
}
