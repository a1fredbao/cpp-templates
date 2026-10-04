#ifndef AFDS_DSU
#define AFDS_DSU

#include <numeric>
#include <vector>

struct DSU {
	std::vector<int> fa, siz;
	DSU(void) = default;
	explicit DSU(int n) : fa(n + 1), siz(n + 1, 1) {
		std::iota(fa.begin(), fa.end(), 0);
	}
	inline void init(int n) {
		n++, fa.resize(n), siz.assign(n, 1);
		std::iota(fa.begin(), fa.end(), 0);
	}
	inline int find(int x) { return fa[x] == x ? x : fa[x] = find(fa[x]); }
	inline bool same(int x, int y) { return find(x) == find(y); }
	inline int size(int u) { return siz[find(u)]; }
	// true if x and y were not in the same set, false otherwise.
	inline bool merge(int x, int y) {
		int fx = find(x), fy = find(y);
		if (fx == fy) return false;
		if (siz[fx] < siz[fy]) std::swap(fx, fy);
		fa[fy] = fx, siz[fx] += siz[fy], siz[fy] = 0;
		return true;
	}
	// x -> y, a.k.a let x be son of y (disable merge by rank).
	inline bool directed_merge(int x, int y) {
		int fx = find(x), fy = find(y);
		if (fx == fy) return false;
		fa[fx] = fy, siz[fy] += siz[fx], siz[fx] = 0;
		return true;
	}
};

#endif // AFDS_DSU
