#pragma once

#include "dinic.hpp"
#include <cassert>
#include <utility>
#include <vector>

class BipartiteMatching {
public:
	explicit BipartiteMatching(int left, int right)
	    : left_(left), right_(right), sink_(left + right + 1),
	      flow_(left + right + 2, left + right) {
		for (int u = 0; u < left_; u++) {
			flow_.add(0, u + 1, 1);
		}
		for (int v = 0; v < right_; v++) {
			flow_.add(left_ + v + 1, sink_, 1);
		}
	}

	void add_edge(int left_vertex, int right_vertex) {
		assert(!solved_);
		assert(0 <= left_vertex && left_vertex < left_);
		assert(0 <= right_vertex && right_vertex < right_);
		flow_.add(left_vertex + 1, left_ + right_vertex + 1, 1);
	}

	int max_matching() {
		if (!solved_) {
			matching_size_ = flow_.maxflow(0, sink_);
			solved_ = true;
		}
		return matching_size_;
	}

	std::vector<std::pair<int, int>> matching() {
		max_matching();
		std::vector<std::pair<int, int>> result;
		for (auto edge : flow_.edges()) {
			bool left_edge = 1 <= edge.from && edge.from <= left_;
			bool right_edge = left_ + 1 <= edge.to && edge.to <= left_ + right_;
			if (left_edge && right_edge && edge.flow == 1) {
				result.push_back({edge.from - 1, edge.to - left_ - 1});
			}
		}
		return result;
	}

private:
	int left_;
	int right_;
	int sink_;
	bool solved_ = false;
	int matching_size_ = 0;
	MaxFlow<int> flow_;
};
