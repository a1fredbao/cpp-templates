#pragma once

#include <array>
#include <cassert>
#include <utility>
#include <vector>

template <class T, const int V>
struct BinaryAlarm {
	int n;
	struct Alarm {
		int h;
		T need, rem;
		std::vector<int> pos;
	};

	std::vector<int> res;
	std::vector<Alarm> alm;
	std::vector<T> a, on, tmp;
	std::vector<std::array<std::vector<int>, V>> L;

	explicit BinaryAlarm(int n) : n(n), a(n), on(n), tmp(V), L(n) {}

	inline T full(int h) { return (T(1) << h) - 1; }
	inline T next(T v, int h) { return ((v >> h) + 1) << h; }

	inline void rebuild(int id) {
		T tot = 0;
		for (int p : alm[id].pos) tot += a[p];
		if (tot >= alm[id].need) {
			alm[id].h = -1;
			res.push_back(id);
			return;
		}
		alm[id].rem = alm[id].need - tot;
		while (full(alm[id].h) * T(alm[id].pos.size()) >= alm[id].rem) {
			alm[id].h--;
		}
		assert(alm[id].h >= 0);
		for (int p : alm[id].pos) {
			on[p] |= T(1) << alm[id].h;
			L[p][alm[id].h].push_back(id);
			alm[id].rem -= next(a[p], alm[id].h) - 1 - a[p];
		}
	}

	inline int monitor(const std::vector<int> &pos, T need) {
		alm.push_back({V - 1, need, T(0), pos});
		int id = alm.size() - 1;
		if (need != 0) {
			for (int p : pos) alm[id].need += a[p];
			rebuild(id);
		} else {
			res.push_back(id);
		}
		return id;
	}

	// a[pos] += v; v must be non-negative.
	inline void increase(int pos, T v) {
		if (v == 0) return;
		std::vector<int> mod;
		T hs = full(std::__lg(a[pos] ^ (a[pos] + v)) + 1) & on[pos];
		on[pos] ^= hs;
		while (hs != 0) {
			int h = std::__lg(hs);
			for (int id : L[pos][h]) {
				if (alm[id].h == h) mod.push_back(id);
			}
			hs ^= T(1) << h, L[pos][h].clear();
			tmp[h] = next(a[pos] + v, h) - next(a[pos], h);
		}
		a[pos] += v;
		for (int id : mod) {
			T wt = tmp[alm[id].h];
			if (alm[id].rem > wt) {
				alm[id].rem -= wt;
				L[pos][alm[id].h].push_back(id);
				on[pos] |= T(1) << alm[id].h;
			} else {
				rebuild(id);
			}
		}
	}

	inline std::vector<int> fetch(void) {
		return std::exchange(res, {});
	}
};
