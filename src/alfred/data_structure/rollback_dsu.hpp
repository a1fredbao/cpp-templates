#ifndef AFDS_CANCEL_DSU
#define AFDS_CANCEL_DSU

#include <numeric>
#include <vector>

class CancelDSU {
private:
	std::vector<int> fa, siz, S;

public:
	CancelDSU(void) = default;
	CancelDSU(int n) : fa(n + 1), siz(n + 1, 1) {
		std::iota(fa.begin(), fa.end(), 0);
	}
	inline void init(int n) {
		n++, fa.resize(n), siz.assign(n, 1);
		std::iota(fa.begin(), fa.end(), 0);
	}
	inline int find(int x) { return fa[x] == x ? x : find(fa[x]); }
	inline bool same(int x, int y) { return find(x) == find(y); }
	inline int size(int u) { return siz[find(u)]; }
	inline void merge(int u, int v) {
		int fu = find(u), fv = find(v);
		if (fu == fv) return S.push_back(-1);
		if (siz[fu] < siz[fv]) std::swap(fu, fv);
		siz[fu] += siz[fv], fa[fv] = fu, S.push_back(fv);
	}
	inline void _cancel(void) {
		if (S.empty()) return;
		if (S.back() == -1) return S.pop_back();
		siz[fa[S.back()]] -= siz[S.back()];
		fa[S.back()] = S.back(), S.pop_back();
	}
	inline void cancel(int t = 1) {
		while (t--) _cancel();
	}
};

#endif // AFDS_CANCEL_DSU
