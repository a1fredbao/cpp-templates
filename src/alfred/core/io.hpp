#pragma once

#include <iostream>
#include <vector>

inline void optimizeIO(void) {
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL), std::cout.tie(NULL);
}

template <class T>
inline void write_vec(std::vector<T> vec, bool in_line = false) {
	std::cout << vec.size() << "\n "[in_line];
	for (T &x : vec) {
		std::cout << x << ' ';
	}
	std::cout << '\n';
}

// TODO: fast io.
