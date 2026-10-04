#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace alfred::debug {

inline constexpr const char *kReset = "\033[0m";
inline constexpr const char *kKey = "\033[36m";
inline constexpr const char *kValue = "\033[33m";
inline constexpr const char *kMuted = "\033[90m";

template <class T>
void print_value(std::ostream &os, const T &value);

inline void print_raw(std::ostream &os, bool value) {
	os << (value ? "true" : "false");
}

inline void print_raw(std::ostream &os, char value) {
	if (std::isprint(static_cast<unsigned char>(value))) {
		os << '\'' << value << '\'';
	} else {
		os << "'\\x" << std::hex
		   << static_cast<int>(static_cast<unsigned char>(value)) << std::dec
		   << '\'';
	}
}

inline void print_raw(std::ostream &os, const std::string &value) {
	os << '"' << value << '"';
}

inline void print_raw(std::ostream &os, const char *value) {
	os << '"' << value << '"';
}

template <class T>
void print_raw(std::ostream &os, const T &value) {
	if constexpr (std::is_enum_v<T>) {
		os << static_cast<std::underlying_type_t<T>>(value);
	} else {
		os << value;
	}
}

template <class A, class B>
void print_value(std::ostream &os, const std::pair<A, B> &value);

template <class... Ts>
void print_value(std::ostream &os, const std::tuple<Ts...> &value);

template <class T, std::size_t N>
void print_value(std::ostream &os, const std::array<T, N> &value);

template <class T, class Alloc>
void print_value(std::ostream &os, const std::vector<T, Alloc> &value);

template <class T, class Alloc>
void print_value(std::ostream &os, const std::deque<T, Alloc> &value);

template <class T, class Alloc>
void print_value(std::ostream &os, const std::list<T, Alloc> &value);

template <class T, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::set<T, Cmp, Alloc> &value);

template <class T, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::multiset<T, Cmp, Alloc> &value);

template <class T, class Hash, class Eq, class Alloc>
void print_value(
    std::ostream &os, const std::unordered_set<T, Hash, Eq, Alloc> &value
);

template <class K, class V, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::map<K, V, Cmp, Alloc> &value);

template <class K, class V, class Hash, class Eq, class Alloc>
void print_value(
    std::ostream &os, const std::unordered_map<K, V, Hash, Eq, Alloc> &value
);

template <class T>
void print_value(std::ostream &os, const std::optional<T> &value);

template <class T>
void print_value(std::ostream &os, const std::vector<bool> &value);

template <std::size_t N>
void print_value(std::ostream &os, const std::bitset<N> &value) {
	os << value;
}

template <class It>
void print_range(std::ostream &os, It first, It last) {
	os << '[';
	bool started = false;
	for (; first != last; ++first) {
		if (started) { os << ", "; }
		started = true;
		print_value(os, *first);
	}
	os << ']';
}

template <class A, class B>
void print_value(std::ostream &os, const std::pair<A, B> &value) {
	os << '(';
	print_value(os, value.first);
	os << ", ";
	print_value(os, value.second);
	os << ')';
}

template <class Tuple, std::size_t... I>
void print_tuple(
    std::ostream &os, const Tuple &value, std::index_sequence<I...>
) {
	os << '(';
	((I == 0 ? void() : void(os << ", "), print_value(os, std::get<I>(value))),
	 ...);
	os << ')';
}

template <class... Ts>
void print_value(std::ostream &os, const std::tuple<Ts...> &value) {
	print_tuple(os, value, std::index_sequence_for<Ts...>{});
}

template <class T, std::size_t N>
void print_value(std::ostream &os, const std::array<T, N> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Alloc>
void print_value(std::ostream &os, const std::vector<T, Alloc> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Alloc>
void print_value(std::ostream &os, const std::deque<T, Alloc> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Alloc>
void print_value(std::ostream &os, const std::list<T, Alloc> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::set<T, Cmp, Alloc> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::multiset<T, Cmp, Alloc> &value) {
	print_range(os, value.begin(), value.end());
}

template <class T, class Hash, class Eq, class Alloc>
void print_value(
    std::ostream &os, const std::unordered_set<T, Hash, Eq, Alloc> &value
) {
	print_range(os, value.begin(), value.end());
}

template <class K, class V, class Cmp, class Alloc>
void print_value(std::ostream &os, const std::map<K, V, Cmp, Alloc> &value) {
	os << '{';
	bool started = false;
	for (const auto &[key, val] : value) {
		if (started) { os << ", "; }
		started = true;
		print_value(os, key);
		os << ": ";
		print_value(os, val);
	}
	os << '}';
}

template <class K, class V, class Hash, class Eq, class Alloc>
void print_value(
    std::ostream &os, const std::unordered_map<K, V, Hash, Eq, Alloc> &value
) {
	os << '{';
	bool started = false;
	for (const auto &[key, val] : value) {
		if (started) { os << ", "; }
		started = true;
		print_value(os, key);
		os << ": ";
		print_value(os, val);
	}
	os << '}';
}

template <class T>
void print_value(std::ostream &os, const std::optional<T> &value) {
	if (value) {
		print_value(os, *value);
	} else {
		os << "nullopt";
	}
}

template <class T>
void print_value(std::ostream &os, const std::vector<bool> &value) {
	os << '[';
	for (std::size_t i = 0; i < value.size(); ++i) {
		if (i) { os << ", "; }
		os << (value[i] ? '1' : '0');
	}
	os << ']';
}

template <class T>
void print_value(std::ostream &os, const T &value) {
	print_raw(os, value);
}

template <class... Ts>
void print_values(std::ostream &os, const Ts &...values) {
	std::size_t index = 0;
	auto emit = [&](const auto &value) {
		if (index++ != 0) { os << ", "; }
		print_value(os, value);
	};
	(emit(values), ...);
}

class Stopwatch {
public:
	void reset() { start_ = clock::now(); }

	double seconds() const {
		return std::chrono::duration<double>(clock::now() - start_).count();
	}

	long long milliseconds() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(
		           clock::now() - start_
		)
		    .count();
	}

private:
	using clock = std::chrono::steady_clock;
	clock::time_point start_ = clock::now();
};

} // namespace alfred::debug

#ifdef ALFRED_DEBUG

#define ALFRED_DBG(...)                                                        \
	do {                                                                       \
		std::cerr << alfred::debug::kKey << "[" << __FILE__ << ":" << __LINE__ \
		          << "] " << alfred::debug::kValue;                            \
		alfred::debug::print_values(std::cerr, __VA_ARGS__);                   \
		std::cerr << alfred::debug::kReset << '\n';                            \
	} while (false)

#define ALFRED_TRACE(...) ALFRED_DBG(__VA_ARGS__)

#define ALFRED_ASSERT(condition)                                               \
	do {                                                                       \
		if (!(condition)) {                                                    \
			std::cerr << alfred::debug::kKey                                   \
			          << "assert failed: " << #condition << " at " << __FILE__ \
			          << ":" << __LINE__ << alfred::debug::kReset << '\n';     \
			std::abort();                                                      \
		}                                                                      \
	} while (false)

#else

#define ALFRED_DBG(...)          ((void)0)
#define ALFRED_TRACE(...)        ((void)0)
#define ALFRED_ASSERT(condition) ((void)0)

#endif
