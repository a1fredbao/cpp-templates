#pragma once

#include "../algorithm/discretization.hpp"

#include <algorithm>
#include <vector>

template <class T>
struct AppearStats {
	Mess<T> M;
	std::vector<std::vector<int>> pos;

	AppearStats(void) = default;
	explicit AppearStats(const std::vector<T> &a) { init(a); }

	inline void init(const std::vector<T> &a) {
		M.clear();
		for (const auto &x : a) M.insert(x);
		M.init(), pos.assign(M.size(), {});
		for (int i = 0; i < int(a.size()); i++) {
			pos[M.query(a[i]) - 1].push_back(i);
		}
	}

	// base is the first legal index; return -1 if x is absent.
	inline int first(int l, int r, T x, int base = 0) {
		l -= base, r -= base;
		if (!M.exist(x)) return -1;
		const auto &p = pos[M.query(x) - 1];
		auto it = std::lower_bound(p.begin(), p.end(), l);
		return it == p.end() || *it > r ? -1 : *it + base;
	}

	inline int last(int l, int r, T x, int base = 0) {
		l -= base, r -= base;
		if (!M.exist(x)) return -1;
		const auto &p = pos[M.query(x) - 1];
		auto it = std::upper_bound(p.begin(), p.end(), r);
		return it == p.begin() || *std::prev(it) < l ? -1 : *std::prev(it) + base;
	}

	inline int count(int l, int r, T x, int base = 0) {
		l -= base, r -= base;
		if (l > r || !M.exist(x)) return 0;
		const auto &p = pos[M.query(x) - 1];
		auto L = std::lower_bound(p.begin(), p.end(), l);
		auto R = std::upper_bound(p.begin(), p.end(), r);
		return int(R - L);
	}
};
