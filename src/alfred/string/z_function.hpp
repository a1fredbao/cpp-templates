#pragma once

#include <string>
#include <vector>

// Returns a vector `z` where z[i] equals to lcp(s, s[i..n))
std::vector<int> z_function(const std::string &s) {
	int n = s.size(), l = 0, r = 1;
	std::vector<int> z(n, 0);
	for (int i = 1; i < n; i++) {
		// s[i, r) = s[i - l, r - l)
		// z[i] >= min(z[i - l], r - i)
		if (i < r && z[i - l] < r - i) {
			z[i] = z[i - l];
		} else {
			if (r > i) z[i] = r - i;
			while (i + z[i] < n && s[i + z[i]] == s[z[i]]) z[i]++;
		}
		if (i + z[i] > r) l = i, r = i + z[i];
	}
	return z[0] = n, z;
}
