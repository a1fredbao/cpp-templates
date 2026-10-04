#ifndef AFSTR_HASHED_STRING
#define AFSTR_HASHED_STRING

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace ctrandom {
constexpr int MOD_POOL[] = {998244353, 1000000007, 1004535809, 1007681537};
constexpr int SEED_POOL[] = {233, 477, 997, 1009};

constexpr std::size_t POOL_SIZE = sizeof(MOD_POOL) / sizeof(int);

// 简单的编译时字符串哈希（BKDR）
constexpr uint32_t ct_str_hash(const char *s) {
	uint32_t h = 0;
	for (; *s; ++s) {
		h = h * 131u + static_cast<uint8_t>(*s);
	}
	return h;
}

// 利用 __TIME__ 挑第一个索引
constexpr std::size_t pick_idx1(void) {
	return ct_str_hash(__TIME__) % POOL_SIZE;
}
// 利用 __DATE__ 再挑一个，并且保证 != idx1()
constexpr std::size_t pick_idx2(void) {
	// 在池中剔除 idx1，令它与 idx1 不重合
	uint32_t h2 = ct_str_hash(__DATE__);
	std::size_t x = h2 % POOL_SIZE;
	if (x == pick_idx1()) x = (x + 1) % POOL_SIZE;
	return x;
}
}

template <int mod, int seed>
struct SingleHash {
	int n;
	std::vector<int> pow, h;
	SingleHash(void) = default;
	SingleHash(std::string &s) { init(s); }
	inline void init(std::string &s) {
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
	SingleHash<
	    ctrandom::MOD_POOL[ctrandom::pick_idx1()],
	    ctrandom::SEED_POOL[ctrandom::pick_idx1()]>
	    H1;
	SingleHash<
	    ctrandom::MOD_POOL[ctrandom::pick_idx2()],
	    ctrandom::SEED_POOL[ctrandom::pick_idx2()]>
	    H2;
	HashedString(void) = default;
	HashedString(std::string s) : H1(s), H2(s) {}
	inline void init(std::string s) { H1.init(s), H2.init(s); }
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
	PalindromeCheck(std::string s) : n(s.size()), H1(s) {
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
