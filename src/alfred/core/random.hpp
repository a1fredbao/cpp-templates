#pragma once

#include <cstdint>
#include <random>
#include <vector>

namespace ctrandom {

constexpr uint32_t ctrandom_hash(const char *s) {
	uint32_t hash = 0;
	for (; *s; s++) {
		hash = hash * 131u + static_cast<uint8_t>(*s);
	}
	return hash;
}

constexpr uint32_t splitmix32(uint32_t x) {
	x += 0x9e3779b9u;
	x = (x ^ (x >> 16)) * 0x85ebca6bu;
	x = (x ^ (x >> 13)) * 0xc2b2ae35u;
	return x ^ (x >> 16);
}

constexpr uint64_t random_seed() {
#ifdef LOCAL
	return 0x4d595df4d0f33173ull;
#else
	uint64_t date = ctrandom_hash(__DATE__);
	uint64_t time = ctrandom_hash(__TIME__);
	return date ^ (time << 21) ^ (time >> 7) ^ 0x9e3779b97f4a7c15ull;
#endif
}

constexpr uint32_t pick(uint32_t bound, uint32_t salt = 0) {
	return splitmix32(static_cast<uint32_t>(random_seed() >> 32) ^ salt) %
	       bound;
}

} // namespace ctrandom

template <class T>
struct Splitmix {
	inline T operator()(T x) {
		if (sizeof(T) == 8) {
			T z = (x += 0x9e3779b97f4a7c15);
			z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
			z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
			return z ^ (z >> 31);
		} else {
			T z = (x += 0x9e3779b9);
			z = (z ^ (z >> 16)) * 0x85ebca6b;
			z = (z ^ (z >> 13)) * 0xc2b2ae35;
			return z ^ (z >> 16);
		}
	}
};

template <>
struct Splitmix<unsigned long long> {
	unsigned long long z;
	inline unsigned long long operator()(unsigned long long x) {
		z = (x += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}
};

template <>
struct Splitmix<unsigned int> {
	unsigned int z;
	inline unsigned int operator()(unsigned int x) {
		z = (x += 0x9e3779b9);
		z = (z ^ (z >> 16)) * 0x85ebca6b;
		z = (z ^ (z >> 13)) * 0xc2b2ae35;
		return z ^ (z >> 16);
	}
};

template <class T>
std::vector<T> rand_seq(size_t n, size_t seed = 20090913) {
	std::vector<T> res(n);
	std::mt19937_64 rng(seed);
	for (size_t i = 0; i < n; i++) {
		res[i] = rng();
	}
	return res;
}
