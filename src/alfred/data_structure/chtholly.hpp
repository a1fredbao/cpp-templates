#pragma once

#include <cassert>
#include <set>
#include <utility>

// Intervals are stored as [l, r).
template <class T>
struct ChthollyTree {
	using i64 = long long;
	struct Node {
		i64 l, r;
		mutable T v;
	};
	struct Cmp {
		bool operator()(const Node &a, const Node &b) const {
			return a.l < b.l;
		}
	};

	i64 n = 0;
	std::set<Node, Cmp> tr;
	using it = typename std::set<Node, Cmp>::iterator;

	ChthollyTree(void) = default;
	explicit ChthollyTree(i64 n, T v = T{}) { init(n, v); }
	inline void init(i64 n, T v = T{}) {
		this->n = n, tr.clear();
		if (n > 0) tr.insert({0, n, v});
	}

	inline it split(i64 p) {
		assert(0 <= p && p <= n);
		if (p == n) return tr.end();
		auto it = tr.lower_bound({p, 0, T{}});
		if (it == tr.end() || it->l > p) --it;
		if (it->l == p) return it;
		auto old = *it;
		tr.erase(it);
		tr.insert({old.l, p, old.v});
		return tr.insert({p, old.r, old.v}).first;
	}

	inline void assign(i64 l, i64 r, T v) {
		assert(0 <= l && l < r && r <= n);
		auto R = split(r), L = split(l);
		tr.erase(L, R), tr.insert({l, r, v});
	}

	template <class F>
	inline void modify(i64 l, i64 r, F f) {
		assert(0 <= l && l < r && r <= n);
		auto R = split(r);
		for (auto it = split(l); it != R; it++) f(it);
	}

	template <class F>
	inline T query(i64 l, i64 r, F f) {
		assert(0 <= l && l < r && r <= n);
		T ans = T{};
		auto R = split(r);
		for (auto it = split(l); it != R; it++) f(ans, it);
		return ans;
	}

	inline it begin(void) { return tr.begin(); }
	inline it end(void) { return tr.end(); }
};
