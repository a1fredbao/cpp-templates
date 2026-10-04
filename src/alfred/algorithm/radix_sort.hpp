#pragma once

#include <cstring> // for memset
#include <type_traits>
#include <vector>

#define _SORTBYTE(TYPE, FR, TO, LEN, BIT, W)                                   \
	memset(bucket, 0, sizeof(bucket));                                         \
	for (TYPE *it = (FR) + LEN; it != (FR); it--)                              \
		++bucket[(it[-1] >> BIT) & ((1 << W) - 1)];                            \
	for (unsigned *it = bucket; it != bucket + ((1 << W) - 1); it++)           \
		it[1] += it[0];                                                        \
	for (TYPE *it = (FR) + LEN; it != (FR); it--)                              \
		(TO)[--bucket[(it[-1] >> BIT) & ((1 << W) - 1)]] = it[-1];

template <class T, const unsigned w = 4>
static void radix_sort(T a[], const size_t n) {
	static std::vector<T> b_vec;
	if (b_vec.size() < n) {
		b_vec.resize(n);
	}
	T *b = b_vec.data();
	static unsigned bucket[1 << w];
	const unsigned tot = sizeof(T) * 8;
	static_assert(
	    tot % (2 * w) == 0, "The width of type must be divisible by 2w."
	);
	for (unsigned d1 = 0, d2 = w; d2 < tot;) {
		_SORTBYTE(T, a, b, n, d1, w);
		_SORTBYTE(T, b, a, n, d2, w);
		d1 += w * 2, d2 += w * 2;
	}
}
