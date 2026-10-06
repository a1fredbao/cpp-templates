#pragma once

#include <functional>
#include <utility>
#include <vector>

// Returns (root, children), where children[i] = (left, right).
template <class T, class Cmp = std::less<T>>
std::pair<int, std::vector<std::pair<int, int>>>
cartesian(std::vector<T> a, Cmp cmp = Cmp()) {
	int n = a.size();
	std::vector<std::pair<int, int>> ch(n, {-1, -1});
	std::vector<int> st;
	st.reserve(n);
	for (int i = 0; i < n; i++) {
		int lst = -1;
		while (!st.empty() && cmp(a[i], a[st.back()])) {
			lst = st.back(), st.pop_back();
		}
		if (!st.empty()) ch[st.back()].second = i;
		ch[i].first = lst, st.push_back(i);
	}
	return {n ? st.front() : -1, ch};
}
