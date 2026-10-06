// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/associative_array

#include "../../../src/alfred/core/fast_io.hpp"
#include "../../../src/alfred/data_structure/hashmap.hpp"

int main() {
	int q, opt;
	unsigned long long x, v;
	fast_read(q);

	HashMap<unsigned long long, unsigned long long> mp;
	mp.init(q);
	while (q--) {
		fast_read(opt, x);
		if (opt == 0) {
			fast_read(v), mp.set(x, v);
		} else {
			writeln(mp.get(x));
		}
	}
	return 0;
}
