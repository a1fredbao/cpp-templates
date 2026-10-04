#ifndef AFDS_BINARY_TRIE
#define AFDS_BINARY_TRIE

// TODO: Require to be rewritten.
// Thanks neal for this template.
#include <array>
#include <cstdint>
#include <vector>

const int BITS = 30;
const int INF = 1e9 + 7;
struct BinaryTrie { // 01-Trie
	static const int ALPHABET = 2;
	struct Node {
		const int parent;
		int words_here = 0;    // How many words EXACTLY here.
		int starting_with = 0; // How many words have the PREFIX of this node.
		int min_index =
		    INF; // The minimum index of words which have PREFIX of this node.
		int max_index =
		    -INF; // The maximum index of words which have PREFIX of this node.
		std::array<int, ALPHABET> child;
		Node(int p = -1) : parent(p) { child.fill(-1); }
	};
	static const int ROOT = 0;
	std::vector<Node> tr = {Node()};
	BinaryTrie(int total_length = -1) { // Sum of |s|, leave -1 if don't know.
		if (total_length >= 0) tr.reserve(total_length + 1);
	}
	// Returns the Node reference of word.
	// NOTICE: this function creates a new Node if word isn't in the trie.
	Node &operator[](uint64_t word) { return tr[build(word, 0)]; }
	// Get or create c-th (c = 0, 1) child of node
	// Returns BinaryTrie node.
	int get_or_create_child(int node, int c) {
		if (tr[node].child[c] == -1) {
			tr[node].child[c] = (int)tr.size();
			tr.push_back(Node(node));
		}
		return tr[node].child[c];
	}
	// Build rootpath of word, insert delta (个) words
	// Returns BinaryTrie node.
	int build(uint64_t word, int delta) {
		int node = ROOT;
		for (int i = BITS - 1; i >= 0; i--) {
			tr[node].starting_with += delta;
			node = get_or_create_child(node, word >> i & 1);
		}
		tr[node].starting_with += delta;
		return node;
	}
	// Insert a word with the index of index, INF if index is unknown.
	// Returns BinaryTrie node.
	int insert(uint64_t word, int index = INF) {
		int node = build(word, 1);
		tr[node].words_here += 1;
		for (int x = node; x != -1; x = tr[x].parent) {
			if (index != INF) {
				tr[x].min_index = std::min(tr[x].min_index, index);
				tr[x].max_index = std::max(tr[x].max_index, index);
			}
		}
		return node;
	}
	// Find such an x inserted in the trie that word ^ x is minimized.
	// Returns such x (x is certain).
	uint64_t query_min(uint64_t word) {
		int node = ROOT;
		uint64_t val = 0;
		for (int i = BITS - 1; i >= 0; i--) {
			int go_bit = word >> i & 1;
			if (tr[node].child[go_bit] == -1) { go_bit ^= 1; }
			val |= 1ull << go_bit;
			node = tr[node].child[go_bit];
		}
		return val;
	}
	// Find such an x inserted in the trie that word ^ x is maximized.
	// Returns such x (x is certain).
	uint64_t query_max(uint64_t word) {
		int node = ROOT;
		uint64_t val = 0;
		for (int i = BITS - 1; i >= 0; i--) {
			int go_bit = (word >> i & 1) ^ 1;
			if (tr[node].child[go_bit] == -1) { go_bit ^= 1; }
			val |= 1ull << go_bit;
			node = tr[node].child[go_bit];
		}
		return val;
	}
	// CF1983F: Find such an x inserted in the trie that word ^ x < upper_bound
	// Returns a pair {min_index, max_index} of x.
	std::pair<int, int> query_ub(uint64_t word, uint64_t upper_bound) {
		int mn = INF, mx = -INF, node = ROOT;
		for (int i = BITS - 1; i >= 0; i--) {
			int word_bit = word >> i & 1;      // digit i of word
			int ub_bit = upper_bound >> i & 1; // digit i of ub
			if (ub_bit == 1 && tr[node].child[word_bit] != -1) {
				// if digit i of ub is 1, then we can choose either
				// the subtree of word_bit or word_bit ^ 1.
				mn = std::min(mn, tr[tr[node].child[word_bit]].min_index);
				mx = std::max(mx, tr[tr[node].child[word_bit]].max_index);
			}
			// else if digit i of ub is 0, then we can only choose
			// the subtree of word_bit. (otherwise, we will violate the range)
			node = tr[node].child[word_bit ^ ub_bit];
			if (node == -1) break;
		}
		return {mn, mx};
	}
};

#endif // AFDS_BINARY_TRIE
