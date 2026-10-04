#pragma once
#include <algorithm>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

#include "../data_structure/sparse_table.hpp"

namespace suffix_array_detail {
template <class T>
struct MinInfo {
	T val;
	MinInfo(void) : val(std::numeric_limits<T>::max()) {}
	MinInfo(T x) : val(x) {}
	inline MinInfo operator+(const MinInfo &x) const {
		return {std::min(val, x.val)};
	}
};
} // namespace suffix_array_detail

namespace sa_is_impl {
inline std::vector<int> sa_is(const std::vector<int> &s, int upper) {
	int n = int(s.size());
	if (n == 0) return {};
	if (n == 1) return {0};
	if (n == 2) {
		if (s[0] < s[1]) return {0, 1};
		else return {1, 0};
	}

	std::vector<int> sa(n);
	std::vector<bool> ls(n);
	for (int i = n - 2; i >= 0; i--) {
		ls[i] = (s[i] == s[i + 1]) ? ls[i + 1] : (s[i] < s[i + 1]);
	}

	std::vector<int> sum_l(upper + 1), sum_s(upper + 1);
	for (int i = 0; i < n; i++) {
		if (!ls[i]) sum_s[s[i]]++;
		else sum_l[s[i] + 1]++;
	}
	for (int i = 0; i <= upper; i++) {
		sum_s[i] += sum_l[i];
		if (i < upper) sum_l[i + 1] += sum_s[i];
	}

	auto induce = [&](const std::vector<int> &lms) {
		std::fill(sa.begin(), sa.end(), -1);
		std::vector<int> buf(upper + 1);
		std::copy(sum_s.begin(), sum_s.end(), buf.begin());
		for (auto d : lms) {
			if (d == n) continue;
			sa[buf[s[d]]++] = d;
		}
		std::copy(sum_l.begin(), sum_l.end(), buf.begin());
		sa[buf[s[n - 1]]++] = n - 1;
		for (int i = 0; i < n; i++) {
			int v = sa[i];
			if (v >= 1 && !ls[v - 1]) {
				sa[buf[s[v - 1]]++] = v - 1;
			}
		}
		std::copy(sum_l.begin(), sum_l.end(), buf.begin());
		for (int i = n - 1; i >= 0; i--) {
			int v = sa[i];
			if (v >= 1 && ls[v - 1]) {
				sa[--buf[s[v - 1] + 1]] = v - 1;
			}
		}
	};

	std::vector<int> lms_map(n + 1, -1);
	int m = 0;
	for (int i = 1; i < n; i++) {
		if (!ls[i - 1] && ls[i]) {
			lms_map[i] = m++;
		}
	}
	std::vector<int> lms;
	lms.reserve(m);
	for (int i = 1; i < n; i++) {
		if (!ls[i - 1] && ls[i]) {
			lms.push_back(i);
		}
	}

	induce(lms);

	if (m) {
		std::vector<int> sorted_lms;
		sorted_lms.reserve(m);
		for (int v : sa) {
			if (lms_map[v] != -1) sorted_lms.push_back(v);
		}
		std::vector<int> rec_s(m);
		int rec_upper = 0;
		rec_s[lms_map[sorted_lms[0]]] = 0;
		for (int i = 1; i < m; i++) {
			int l = sorted_lms[i - 1], r = sorted_lms[i];
			int end_l = (lms_map[l] + 1 < m) ? lms[lms_map[l] + 1] : n;
			int end_r = (lms_map[r] + 1 < m) ? lms[lms_map[r] + 1] : n;
			bool same = true;
			if (end_l - l != end_r - r) {
				same = false;
			} else {
				while (l < end_l) {
					if (s[l] != s[r]) {
						break;
					}
					l++;
					r++;
				}
				if (l == n || s[l] != s[r]) same = false;
			}
			if (!same) rec_upper++;
			rec_s[lms_map[sorted_lms[i]]] = rec_upper;
		}

		auto rec_sa = sa_is(rec_s, rec_upper + 1);

		for (int i = 0; i < m; i++) {
			sorted_lms[i] = lms[rec_sa[i]];
		}
		induce(sorted_lms);
	}
	return sa;
}

inline std::vector<int> suffix_array(const std::string &s) {
	int n = int(s.size());
	std::vector<int> s2(n);
	for (int i = 0; i < n; i++) {
		s2[i] = (unsigned char)s[i];
	}
	return sa_is(s2, 256);
}
} // namespace sa_is_impl

class SuffixArray {
private:
	int n;
	std::string s;
	SparseTable<suffix_array_detail::MinInfo<int>> ST;
	std::vector<int> sa, rnk, h;

public:
	SuffixArray(const std::string &S) : n(int(S.size())), s(S) {
		if (n == 0) return;
		sa = sa_is_impl::suffix_array(s);
		rnk.assign(n, 0);
		for (int i = 0; i < n; i++) {
			rnk[sa[i]] = i;
		}
		h.assign(n, 0);
		for (int i = 0, k = 0; i < n; i++) {
			if (rnk[i] == 0) continue;
			if (k != 0) k--;
			int j = sa[rnk[i] - 1];
			while (i + k < n && j + k < n && s[i + k] == s[j + k]) {
				k++;
			}
			h[rnk[i]] = k;
		}
		ST.init(h);
	}
	const std::vector<int> &get_h(void) const { return h; }
	const std::vector<int> &get_sa(void) const { return sa; }
	const std::vector<int> &get_rnk(void) const { return rnk; }
	inline int lcp(int i, int j) { // 0-indexed
		if (i < 0 || j < 0 || i >= n || j >= n) return 0;
		if (i == j) return n - i;
		i = rnk[i], j = rnk[j];
		if (i > j) std::swap(i, j);
		return ST.query(i + 1, j).val;
	}
};
