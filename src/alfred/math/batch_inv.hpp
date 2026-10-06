#pragma once

#include <vector>

template <class T>
inline std::vector<T> batch_inv(const std::vector<T> &a) {
	int n = a.size();
	std::vector<T> pre(n + 1, 1), inv(n);
	for (int i = 0; i < n; i++) {
		pre[i + 1] = pre[i] * (a[i] == 0 ? T(1) : a[i]);
	}
	T cur = n ? pre[n].inv() : T(1);
	for (int i = n - 1; i >= 0; i--) {
		if (a[i] == 0) {
			inv[i] = 0;
		} else {
			inv[i] = cur * pre[i];
			cur *= a[i];
		}
	}
	return inv;
}

template <class T>
struct VecInv {
	std::vector<T> inv;

	explicit VecInv(const std::vector<T> &a) : inv(batch_inv(a)) {}
	inline T query(int i) const { return inv[i]; }
	inline T operator[](int i) const { return inv[i]; }
};
