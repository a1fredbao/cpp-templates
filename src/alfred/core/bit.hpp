#pragma once

inline constexpr int ceil_pow2(int len) {
	int res = 1;
	while (res < len) res *= 2;
	return res;
}
