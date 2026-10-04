// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/staticrmq

#include "../../../src/alfred/core/io.hpp"
#include "../../../src/alfred/data_structure/sparse_table.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

template <class T>
struct MinInfo {
	T val;
	MinInfo(void) : val(std::numeric_limits<T>::max()) {}
	MinInfo(T x) : val(x) {}
	inline MinInfo operator+(const MinInfo &x) const {
		return {std::min(val, x.val)};
	}
};

int n, q, l, r;

int main(int argc, char const *argv[]) {
	optimizeIO(), std::cin >> n >> q;

	std::vector<int> a(n);
	for (auto &x : a) std::cin >> x;

	SparseTable<MinInfo<int>> ST(a);

	while (q--) {
		std::cin >> l >> r;
		std::cout << ST.query(l, r - 1).val << '\n';
	}

	return 0;
}
