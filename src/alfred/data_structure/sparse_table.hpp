#ifndef AFDS_SPARSE_TABLE
#define AFDS_SPARSE_TABLE

#include <numeric>
#include <vector>

template <class T>
class SparseTable {
private:
	int n;
	std::vector<std::vector<T>> ST;

public:
	SparseTable(void) {}
	SparseTable(int N) : n(N), ST(N, std::vector<T>(std::__lg(N) + 1)) {}
	template <class InitT>
	SparseTable(std::vector<InitT> &_init) : SparseTable(_init.size()) {
		init(_init, true);
	}
	template <class InitT>
	inline void init(std::vector<InitT> &_init, bool internal = false) {
		if (!internal) {
			n = _init.size();
			ST.assign(n, std::vector<T>(std::__lg(n) + 1));
		}
		for (int i = 0; i < n; i++) ST[i][0] = T(_init[i]);
		for (int i = 1; (1 << i) <= n; i++) {
			for (int j = 0; j + (1 << i) - 1 < n; j++) {
				ST[j][i] = ST[j][i - 1] + ST[j + (1 << (i - 1))][i - 1];
			}
		}
	}
	inline T query(int l, int r) { // 0 based
		if (l > r) return T();
		int w = std::__lg(r - l + 1);
		return ST[l][w] + ST[r - (1 << w) + 1][w];
	}
	inline T disjoint_query(int l, int r) {
		if (l > r) return T();
		T ans = T();
		for (int i = std::__lg(r - l + 1); i >= 0; i--) {
			if ((1 << i) <= r - l + 1) {
				ans = ans + ST[l][i];
				l += 1 << i;
			}
		}
		return ans;
	}
};

#endif // AFDS_SPARSE_TABLE
