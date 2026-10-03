#ifndef ALFRED_IO
#define ALFRED_IO

#include "../math/mod-int.hpp"
#include <cstdlib>  // for malloc, free
#include <cstring>  // for memcpy
#include <iostream> // for fwrite, fread, std::string

class FastIO {
private:
    static const int chunk = 1 << 18;
    char *ibuf, *p1, *p2, *obuf, *op, *oe;

    inline void flush_output(int len = chunk) {
        fwrite(obuf, 1, len, stdout), op = obuf;
    }

    inline size_t min(const size_t &a, const size_t &b) { // avoid including <algorithm>
        return a < b ? a : b;
    }

public:
    inline char nc(void) {
        if (p1 == p2) {
            p2 = (p1 = ibuf) + fread(ibuf, 1, chunk, stdin);
            if (p1 == p2) return EOF;
        }
        return *p1++;
    }
    inline void pc(char c) {
        if (op == oe) flush_output();
        *op++ = c;
    }
    inline void ps(const char *s, size_t len) {
        // do memcpy magic here
        size_t d = 0;
        while (len > 0) {
            size_t cp = min(len, oe - op);
            memcpy(op, s + d, cp), d += cp, len -= cp;
            if ((op += cp) == oe) flush_output();
        }
    }
    FastIO(void) {
        p1 = p2 = ibuf = (char *)(malloc(chunk));
        op = obuf = (char *)(malloc(chunk)), oe = obuf + chunk;
    }
    ~FastIO(void) {
        flush_output(op - obuf);
        free(ibuf), free(obuf);
    }
    inline void flush(void) {
        flush_output(op - obuf);
    }
} __buf;

#define getchar()  __buf.nc()
#define putchar(x) __buf.pc(x)

// specialized for unsigned int: no negative sign handling
template <class T, is_unsigned_int_t<T> *_ = nullptr>
inline void fast_read(T &x) {
    x = 0;
    static char c = __buf.nc();
    while (c < '0' || c > '9') c = __buf.nc();
    while (c >= '0' && c <= '9') {
        x = (x << 1) + (x << 3) + (c ^ 48), c = __buf.nc();
    }
}

template <class T, is_signed_int_t<T> *_ = nullptr>
inline void fast_read(T &x) {
    x = 0;
    static bool neg = false;
    static char c = __buf.nc(); // it runs ok though initialized only once
    while (c < '0' || c > '9') {
        neg = (c == '-'), c = __buf.nc();
    }
    while (c >= '0' && c <= '9') {
        x = (x << 1) + (x << 3) + (c ^ 48), c = __buf.nc();
    }
    if (neg) x = -x;
}

inline void fast_read(std::string &s) {
    s.clear();
    static char c = __buf.nc();
    while (c == ' ' || c == '\n') {
        c = __buf.nc();
    }
    while (c != ' ' && c != '\n') {
        s.push_back(c), c = __buf.nc();
    }
}

template <class T>
inline void write(T x) {
    bool neg = false;
    if (x < 0) {
        neg = true, x = -x;
    }
    int it = 40;
    static char buf[40];
    do {
        buf[--it] = (x % 10) ^ 48, x /= 10;
    } while (x);
    if (neg) buf[--it] = '-';
    __buf.ps(buf + it, 40 - it);
}
template <int mod>
inline void write(ModInt<mod> x) { write(x.val()); }
inline void write(const std::string &s) {
    __buf.ps(s.data(), s.size());
}
template <typename T, typename... V>
inline void write(T x, V... v) {
    write(x), __buf.pc(' '), write(v...);
}
template <class T>
inline void writeln(T x) {
    write(x), __buf.pc('\n');
}
template <typename T, typename... V>
inline void writeln(T x, V... v) {
    write(x), __buf.pc(' '), writeln(v...);
}
template <typename T, typename... V>
inline void fast_read(T &t, V &...v) {
    fast_read(t), fast_read(v...);
}

#endif // ALFRED_IO
