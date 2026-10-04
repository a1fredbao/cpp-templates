// competitive-verifier: STANDALONE

#include "../../src/alfred/algorithm/radix_sort.hpp"
#include <cassert>
#include <random>

const int N = 100000000;

uint32_t a[N];
std::mt19937 rng(std::random_device{}());

int main(int argc, char const *argv[]) {
	for (int i = 0; i < N; i++) {
		a[i] = rng();
	}
	radix_sort(a, N);
	for (int i = 1; i < N; i++) {
		assert(a[i - 1] <= a[i]);
	}
	return 0;
}
