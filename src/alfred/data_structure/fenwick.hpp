#pragma once
#include <vector>

template <class T>
struct Fenwick {
	int n;
	std::vector<T> c;
	Fenwick(void) = default;
	explicit Fenwick(int len) : n(len + 2), c(len + 2) {}
	inline void init(int _n) { n = _n + 2, c.assign(n, T()); }
	inline int lowbit(int x) { return x & -x; }
	inline void update(int pos, T x) {
		if (pos < 0) return;
		if (++pos >= n) return;
		for (; pos < n; pos += lowbit(pos)) {
			c[pos] += x;
		}
	}
	inline void clear(void) {
		for (auto &x : c) x = T();
	}
	inline T query(int pos) {
		T ans = T();
		if (pos < 0) return ans;
		if (++pos >= n) pos = n - 1;
		for (; pos; pos ^= lowbit(pos)) {
			ans += c[pos];
		}
		return ans;
	}
	inline T query(int l, int r) { return query(r) - query(l - 1); }
};
