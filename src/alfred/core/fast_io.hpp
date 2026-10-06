#pragma once

#include "types.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

class FastIO {
private:
	static constexpr int BUF = 1 << 18;
	char *ib, *ip, *ie, *ob, *op, *oe;

	inline void flush(int len) {
		if (len) std::fwrite(ob, 1, len, stdout);
		op = ob;
	}

	static inline size_t min(size_t a, size_t b) { return a < b ? a : b; }

public:
	FastIO(void) {
		ip = ie = ib = (char *)std::malloc(BUF);
		op = ob = (char *)std::malloc(BUF), oe = ob + BUF;
	}
	~FastIO(void) {
		flush(int(op - ob));
		std::free(ib), std::free(ob);
	}
	FastIO(const FastIO &) = delete;
	FastIO &operator=(const FastIO &) = delete;

	inline int nc(void) {
		if (ip == ie) {
			ie = (ip = ib) + std::fread(ib, 1, BUF, stdin);
			if (ip == ie) return EOF;
		}
		return (unsigned char)*ip++;
	}
	inline void pc(char c) {
		if (op == oe) flush(BUF);
		*op++ = c;
	}
	inline void ps(const char *s, size_t len) {
		size_t d = 0;
		while (len) {
			size_t cp = min(len, size_t(oe - op));
			std::memcpy(op, s + d, cp);
			d += cp, len -= cp, op += cp;
			if (op == oe) flush(BUF);
		}
	}
	inline void flush(void) { flush(int(op - ob)); }

	template <class T, is_unsigned_int_t<T> *_ = nullptr>
	inline void read(T &x) {
		x = 0;
		int c;
		do {
			c = nc();
			if (c == EOF) return;
		} while (c < '0' || c > '9');
		while (c >= '0' && c <= '9') {
			x = (x << 1) + (x << 3) + (c ^ 48);
			c = nc();
		}
	}
	template <class T, is_signed_int_t<T> *_ = nullptr>
	inline void read(T &x) {
		x = 0;
		bool neg = false;
		int c;
		do {
			c = nc();
			if (c == EOF) return;
		} while (c <= ' ');
		if (c == '-') {
			neg = true;
			c = nc();
		}
		while (c >= '0' && c <= '9') {
			x = (x << 1) + (x << 3) + (c ^ 48);
			c = nc();
		}
		if (neg) x = -x;
	}
	inline void read(std::string &s) {
		s.clear();
		int c;
		do {
			c = nc();
			if (c == EOF) return;
		} while (c <= ' ');
		while (c > ' ') {
			s.push_back(char(c));
			c = nc();
		}
	}

	template <class T, is_unsigned_int_t<T> *_ = nullptr>
	inline void write(T x) {
		char b[40];
		int it = 40;
		do {
			b[--it] = char('0' + x % 10);
			x /= 10;
		} while (x);
		ps(b + it, 40 - it);
	}
	template <class T, is_signed_int_t<T> *_ = nullptr>
	inline void write(T x) {
		using U = std::make_unsigned_t<T>;
		bool neg = x < 0;
		U y = neg ? U(~x) + 1 : U(x);
		char b[40];
		int it = 40;
		do {
			b[--it] = char('0' + y % 10);
			y /= 10;
		} while (y);
		if (neg) b[--it] = '-';
		ps(b + it, 40 - it);
	}
	inline void write(const std::string &s) { ps(s.data(), s.size()); }
	inline void write(const char *s) { ps(s, std::strlen(s)); }
	template <class T, class = decltype(std::declval<const T &>().val())>
	inline void write(const T &x) {
		write(x.val());
	}

	inline void write(void) {}
	template <class T, class... V>
	inline void write(const T &x, const V &...v) {
		write(x);
		if constexpr (sizeof...(v)) {
			pc(' ');
			write(v...);
		}
	}
	template <class T, class... V>
	inline void writeln(const T &x, const V &...v) {
		write(x, v...);
		pc('\n');
	}
};

inline FastIO fio;

template <class T, class... V>
inline void fast_read(T &x, V &...v) {
	fio.read(x);
	if constexpr (sizeof...(v)) fast_read(v...);
}

template <class T, class... V>
inline void write(const T &x, const V &...v) {
	fio.write(x, v...);
}

template <class T, class... V>
inline void writeln(const T &x, const V &...v) {
	fio.writeln(x, v...);
}
