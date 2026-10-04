#ifndef AFSTR_HASHED_STRING
#define AFSTR_HASHED_STRING

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "../core/random.hpp"

constexpr int HASH_MOD_POOL[] = {998244353, 1000000007, 1004535809, 1007681537};
constexpr int HASH_SEED_POOL[] = {233, 477, 997, 1009};
constexpr std::size_t HASH_POOL_SIZE =
    sizeof(HASH_MOD_POOL) / sizeof(HASH_MOD_POOL[0]);

constexpr std::size_t HASH_INDEX1 = ctrandom::pick(HASH_POOL_SIZE, 0);
constexpr std::size_t HASH_INDEX2_RAW = ctrandom::pick(HASH_POOL_SIZE, 1);
constexpr std::size_t HASH_INDEX2 = HASH_INDEX2_RAW == HASH_INDEX1
                                        ? (HASH_INDEX2_RAW + 1) % HASH_POOL_SIZE
                                        : HASH_INDEX2_RAW;
constexpr int HASH_MOD1 = HASH_MOD_POOL[HASH_INDEX1];
constexpr int HASH_MOD2 = HASH_MOD_POOL[HASH_INDEX2];
constexpr int HASH_SEED1 = HASH_SEED_POOL[HASH_INDEX1];
constexpr int HASH_SEED2 = HASH_SEED_POOL[HASH_INDEX2];

template <int mod, int seed>
struct SingleHash {
	int n;
	std::vector<int> pow, h;
	SingleHash(void) = default;
	explicit SingleHash(const std::string &s) { init(s); }
	inline void init(const std::string &s) {
		n = s.size(), h.assign(n + 2, 0), pow.assign(n + 2, 1);
		for (int i = 1; i <= n; i++) {
			pow[i] = 1ll * pow[i - 1] * seed % mod;
			h[i] = (1ll * h[i - 1] * seed + s[i - 1]) % mod;
		}
	}
	inline int get_hash(int l, int r) {
		return (h[r + 1] - 1ll * h[l] * pow[r - l + 1] % mod + mod) % mod;
	}
	inline bool check_same(int l1, int r1, int l2, int r2) {
		return get_hash(l1, r1) == get_hash(l2, r2);
	}
};

struct HashedString {
	SingleHash<HASH_MOD1, HASH_SEED1> H1;
	SingleHash<HASH_MOD2, HASH_SEED2> H2;
	HashedString(void) = default;
	explicit HashedString(const std::string &s) : H1(s), H2(s) {}
	inline void init(const std::string &s) { H1.init(s), H2.init(s); }
	std::pair<int, int> get_hash(int l, int r) { // not recommended.
		return {H1.get_hash(l, r), H2.get_hash(l, r)};
	}
	// caution: index begins with zero.
	// If index beginning with one is wanted, use s = ' ' + s
	inline bool check_same(int l1, int r1, int l2, int r2) {
		return H1.check_same(l1, r1, l2, r2) && H2.check_same(l1, r1, l2, r2);
	}
	inline bool check_period(int l, int r, int p) {
		return check_same(l, r - p, l + p, r);
	}
};

struct PalindromeCheck {
	int n;
	HashedString H1, H2;
	PalindromeCheck(void) = default;
	explicit PalindromeCheck(std::string s) : n(s.size()), H1(s) {
		std::reverse(s.begin(), s.end()), H2.init(s);
	}
	inline void init(std::string s) {
		n = s.size(), H1.init(s);
		std::reverse(s.begin(), s.end()), H2.init(s);
	}
	inline bool check(int l, int r) {
		return H1.get_hash(l, r) == H2.get_hash(n - 1 - r, n - 1 - l);
	}
};

#endif // AFSTR_HASHED_STRING
