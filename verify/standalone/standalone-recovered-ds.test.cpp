// competitive-verifier: STANDALONE

#include "../../src/alfred/data_structure/binary_alarm.hpp"
#include "../../src/alfred/data_structure/cartesian.hpp"
#include "../../src/alfred/data_structure/chtholly.hpp"
#include "../../src/alfred/data_structure/hashmap.hpp"
#include "../../src/alfred/data_structure/priority_set.hpp"
#include <cassert>
#include <functional>
#include <vector>

int main() {
	{
		std::vector<int> a{3, 1, 2};
		auto [root, ch] = cartesian(a);
		assert(root == 1 && ch[1] == std::make_pair(0, 2));
	}
	{
		HashMap<int, long long> h;
		h.init(8);
		h.set(3, 7), h.set(3, 9), h.set(8, 1), h.set(8, 2);
		assert(h.size() == 2);
		assert(h.get(3) == 9 && h.get(8) == 2 && h.get(5) == 0);
		assert(h.contains(3) && !h.contains(5));
		h.clear();
		assert(h.size() == 0 && !h.contains(3));
	}
	{
		PrioritySet<int, std::less<int>> lo;
		PrioritySet<int, std::greater<int>> hi;
		for (int x : {3, 1, 4, 1, 5}) lo.insert(x), hi.insert(x);
		assert(lo.top() == 5 && hi.top() == 1);
		lo.erase(5), hi.erase(5);
		assert(lo.top() == 4 && hi.top() == 1);
		lo.erase(1), hi.erase(1);
		assert(hi.top() == 1 && lo.size() == 3 && hi.size() == 3);
	}
	{
		ChthollyTree<long long> tr(10, 0);
		tr.assign(2, 5, 7);
		tr.assign(4, 8, 3);
		tr.assign(0, 10, 2);
		tr.assign(2, 6, 7);
		tr.assign(4, 8, 3);
		auto sum = tr.query(0, 10, [](long long &ans, auto it) {
			ans += (it->r - it->l) * it->v;
		});
		assert(sum == 2 * 2 + 2 * 7 + 4 * 3 + 2 * 2);
		tr.modify(0, 10, [](auto it) { it->v += 1; });
		sum = tr.query(0, 10, [](long long &ans, auto it) {
			ans += (it->r - it->l) * it->v;
		});
		assert(sum == 2 * 3 + 2 * 8 + 4 * 4 + 2 * 3);
	}
	{
		BinaryAlarm<long long, 4> ba(5);
		int id1 = ba.monitor({0, 1}, 3);
		int id2 = ba.monitor({2}, 5);
		assert(ba.fetch().empty());
		ba.increase(0, 1), ba.increase(1, 1);
		assert(ba.fetch().empty());
		ba.increase(1, 1);
		auto fired = ba.fetch();
		assert(fired.size() == 1 && fired[0] == id1);
		ba.increase(2, 5);
		fired = ba.fetch();
		assert(fired.size() == 1 && fired[0] == id2);
		assert(ba.fetch().empty());
	}
	return 0;
}
