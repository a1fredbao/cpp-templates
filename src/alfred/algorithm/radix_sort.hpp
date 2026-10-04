#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>

template <class T>
inline uint64_t radix_key(T value) {
	static_assert(
	    std::is_integral_v<T> && !std::is_same_v<T, bool>,
	    "radix_sort requires an integral type"
	);
	static_assert(sizeof(T) <= 8, "radix_sort supports integers up to 64 bits");
	using U = std::make_unsigned_t<T>;
	U result = static_cast<U>(value);
	if constexpr (std::is_signed_v<T>) {
		result ^= U(1) << (sizeof(U) * 8 - 1);
	}
	return static_cast<uint64_t>(result);
}

template <class T, unsigned width = 4>
void radix_sort(T a[], size_t n) {
	static_assert(width > 0 && width < 8, "radix width must be in [1, 7]");
	static_assert(
	    (sizeof(T) * 8) % (2 * width) == 0,
	    "the width of the type must be divisible by 2 * width"
	);
	constexpr unsigned bucket_count = 1u << width;
	constexpr uint64_t mask = bucket_count - 1;
	std::vector<T> buffer(n);
	std::array<unsigned, bucket_count> bucket{};
	T *b = buffer.data();

	auto sort_byte = [&](T *from, T *to, unsigned shift) {
		std::fill(bucket.begin(), bucket.end(), 0u);
		for (T *it = from + n; it != from;) {
			--it;
			++bucket[(radix_key(*it) >> shift) & mask];
		}
		for (unsigned i = 1; i < bucket_count; i++) {
			bucket[i] += bucket[i - 1];
		}
		for (T *it = from + n; it != from;) {
			--it;
			to[--bucket[(radix_key(*it) >> shift) & mask]] = *it;
		}
	};

	const unsigned total_bits = sizeof(T) * 8;
	for (unsigned low = 0, high = width; high < total_bits;) {
		sort_byte(a, b, low);
		sort_byte(b, a, high);
		low += width * 2;
		high += width * 2;
	}
}
