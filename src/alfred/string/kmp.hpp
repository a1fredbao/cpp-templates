#pragma once

#include <string>
#include <vector>

std::vector<int> prefix_function(const std::string &s) {
	const int n = s.size();
	std::vector<int> pi(n, 0);
	for (int i = 1; i < n; i++) {
		int j = pi[i - 1];
		while (j && s[j] != s[i]) j = pi[j - 1];
		pi[i] = s[j] == s[i] ? j + 1 : 0;
	}
	return pi;
}

// Find all occurrences of string `t` in string `s` in O(n).
// Returns a vector containing all starting positions of occurrences.
// (i.e. all indices i such that `s[i...i + t.size())` = `t`)
std::vector<int> kmp(const std::string &s, const std::string &t) {
	std::vector<int> ans;
	auto pi = prefix_function(t);
	for (int i = 0, j = 0; i < s.size(); i++) {
		while (j && s[i] != t[j]) j = pi[j - 1];
		if (s[i] == t[j]) j++;
		if (j == t.size()) {
			ans.push_back(i + 1 - t.size());
			j = pi[j - 1]; // avoid accessing t[t.size()]
		}
	}
	return ans;
}
