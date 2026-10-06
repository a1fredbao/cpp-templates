#pragma once

#include "../core/random.hpp"
#include "../core/types.hpp"

#include <cassert>
#include <vector>

// Fixed-capacity integer hash map for extreme constant optimization.
// init(cap): no rehash, no auto growth; caller must guarantee at most cap
// distinct live keys. Use unordered_map when dynamic growth is needed.
template <class K, class V>
struct HashMap {
	using u32 = unsigned;
	struct Node {
		K key;
		V val;
		u32 ne;
	};

	u32 lim, mask, tot, cap;
	Splitmix<to_unsigned_t<K>> hs;
	std::vector<u32> fi;
	std::vector<Node> e;

	HashMap(void) = default;
	explicit HashMap(u32 cap) { init(cap); }

	inline void init(u32 cap) {
		this->cap = cap;
		u32 n = cap + 1;
		lim = 1;
		while (lim < n) lim <<= 1;
		fi.assign(lim, 0), e.resize(n), tot = 0, mask = lim - 1;
	}

	inline u32 hash(K x) { return u32(hs(to_unsigned_t<K>(x))); }

	inline void clear(void) {
		fi.assign(lim, 0), tot = 0;
	}
	inline u32 size(void) const { return tot; }

	inline void set(K x, const V &v) {
		u32 u = hash(x) & mask;
		for (u32 i = fi[u]; i; i = e[i].ne) {
			if (e[i].key == x) {
				e[i].val = v;
				return;
			}
		}
		assert(tot < cap);
		e[++tot] = {x, v, fi[u]}, fi[u] = tot;
	}

	inline V get(K x) {
		u32 u = hash(x) & mask;
		for (u32 i = fi[u]; i; i = e[i].ne) {
			if (e[i].key == x) return e[i].val;
		}
		return V();
	}

	inline bool contains(K x) {
		u32 u = hash(x) & mask;
		for (u32 i = fi[u]; i; i = e[i].ne) {
			if (e[i].key == x) return true;
		}
		return false;
	}

	inline V &operator[](K x) {
		u32 u = hash(x) & mask;
		for (u32 i = fi[u]; i; i = e[i].ne) {
			if (e[i].key == x) return e[i].val;
		}
		assert(tot < cap);
		e[++tot] = {x, V(), fi[u]}, fi[u] = tot;
		return e[tot].val;
	}
};
