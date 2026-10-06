#pragma once

#include <functional>
#include <queue>
#include <vector>

template <class T, class Cmp = std::less<T>>
class PrioritySet {
private:
	std::priority_queue<T, std::vector<T>, Cmp> a, b;

public:
	PrioritySet(void) = default;
	explicit PrioritySet(const std::vector<T> &v) {
		for (const auto &x : v) insert(x);
	}

	inline void insert(const T &x) { a.push(x); }
	inline void erase(const T &x) { b.push(x); }

	inline const T &top(void) {
		while (!b.empty() && a.top() == b.top()) a.pop(), b.pop();
		return a.top();
	}
	inline size_t size(void) const { return a.size() - b.size(); }
	inline bool empty(void) const { return size() == 0; }
};
