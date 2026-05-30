// TODO Next generation of flat geometry.
#ifndef AFMT_FLAT_GEOMETRY
#define AFMT_FLAT_GEOMETRY

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

// Recommended eps is 0.25 * given precision
template <long double eps>
struct Double {
    long double val;

    constexpr Double(long double v = 0) : val(v) {}
    static constexpr int sgn(long double x) {
        return (x > eps) - (x < -eps);
    }
    // constexpr long double val(void) const { return val; }
    constexpr operator long double(void) const { return val; }
    constexpr bool operator==(const Double &rhs) const { return sgn(val - rhs.val) == 0; }
    constexpr bool operator!=(const Double &rhs) const { return sgn(val - rhs.val) != 0; }
    constexpr bool operator<(const Double &rhs) const { return sgn(val - rhs.val) < 0; }
    constexpr bool operator>(const Double &rhs) const { return sgn(val - rhs.val) > 0; }
    constexpr bool operator<=(const Double &rhs) const { return sgn(val - rhs.val) <= 0; }
    constexpr bool operator>=(const Double &rhs) const { return sgn(val - rhs.val) >= 0; }
    constexpr Double operator+(void) const { return *this; }
    constexpr Double operator-(void) const { return Double(-val); }
    constexpr Double &operator+=(const Double &rhs) {
        val += rhs.val;
        return *this;
    }
    constexpr Double &operator-=(const Double &rhs) {
        val -= rhs.val;
        return *this;
    }
    constexpr Double &operator*=(const Double &rhs) {
        val *= rhs.val;
        return *this;
    }
    constexpr Double &operator/=(const Double &rhs) {
        val /= rhs.val;
        return *this;
    }
    friend constexpr Double operator+(Double l, const Double &r) { return l += r; }
    friend constexpr Double operator-(Double l, const Double &r) { return l -= r; }
    friend constexpr Double operator*(Double l, const Double &r) { return l *= r; }
    friend constexpr Double operator/(Double l, const Double &r) { return l /= r; }
    friend std::ostream &operator<<(std::ostream &os, const Double &x) { return os << x.val; }
    friend std::istream &operator>>(std::istream &is, Double &x) { return is >> x.val; }
    friend Double sqrt(const Double &x) { return Double(std::sqrt(x.val)); }
    friend Double abs(const Double &x) { return Double(std::abs(x.val)); }
};

template <class T>
struct Vec {
    T x, y;
    constexpr Vec(T _x = 0, T _y = 0) : x(_x), y(_y) {}
    constexpr T norm(void) const { return x * x + y * y; }
    constexpr Vec operator+(void) const noexcept { return *this; }
    constexpr Vec operator-(void) const noexcept { return Vec(-x, -y); }
    constexpr Vec &operator+=(const Vec &rhs) noexcept {
        return x += rhs.x, y += rhs.y, *this;
    }
    constexpr Vec &operator-=(const Vec &rhs) noexcept {
        return x -= rhs.x, y -= rhs.y, *this;
    }
    constexpr Vec &operator*=(const T &d) noexcept {
        return x *= d, y *= d, *this;
    }
    constexpr Vec &operator/=(const T &d) noexcept {
        return x /= d, y /= d, *this;
    }
    friend constexpr Vec operator+(Vec l, const Vec &r) noexcept { return l += r; }
    friend constexpr Vec operator-(Vec l, const Vec &r) noexcept { return l -= r; }
    friend constexpr Vec operator*(const T &l, Vec r) noexcept { return r *= l; }
    friend constexpr Vec operator*(Vec l, const T &r) noexcept { return l *= r; }
    friend constexpr Vec operator/(Vec l, const T &r) noexcept { return l /= r; }
    friend constexpr bool operator==(const Vec &l, const Vec &r) noexcept {
        return l.x == r.x && l.y == r.y;
    }
    friend constexpr bool operator!=(const Vec &l, const Vec &r) noexcept {
        return l.x != r.x || l.y != r.y;
    }
    friend std::ostream &operator<<(std::ostream &out, const Vec &P) {
        return out << "(" << P.x << ", " << P.y << ")";
    }
    friend std::istream &operator>>(std::istream &in, Vec &P) {
        return in >> P.x >> P.y;
    }
};

template <class T>
using Point = Vec<T>;

template <class T>
inline T dot(const Vec<T> &a, const Vec<T> &b) {
    return a.x * b.x + a.y * b.y;
}

template <class T>
inline T cross(const Vec<T> &a, const Vec<T> &b) {
    return a.x * b.y - a.y * b.x;
}

template <class T>
struct PolarAngleComparator {
    Point<T> o;
    Vec<T> base; // vector (x - o), counterclockwise, from base (included).
    PolarAngleComparator(
        const Point<T> &o = Point<T>(0, 0),
        const Vec<T> &base = Vec<T>(1, 0)
    ) : o(o), base(base) {}

    inline int get_half(const Vec<T> &v) {
        T crs = cross(base, v);
        if (crs > 0) return 0;   // counterclockwise to base
        if (crs < 0) return 1;   // clockwise to base
        return dot(base, v) < 0; // collinear to base, but opposite direction
    }

    inline bool operator()(const Point<T> &a, const Point<T> &b) const {
        const Vec<T> va(a.x - o.x, a.y - o.y);
        const Vec<T> vb(b.x - o.x, b.y - o.y);
        int ha = get_half(va), hb = get_half(vb);
        return ha != hb ? ha < hb : cross(va, vb) > 0;
    }
};

template <class T>
std::vector<Point<T>> convex_hull(std::vector<Point<T>> P) { // graham scan, O(n log n)
    if (P.size() <= 1) return P;

    std::vector<Point<T>> H;
    std::swap(P[0], *std::min_element(P.begin(), P.end()));
    std::sort(P.begin() + 1, P.end(), PolarAngleComparator<T>(P[0]));
    for (const auto &p : P) {
        while (H.size() >= 2 && cross(H.back() - H[H.size() - 2], p - H.back()) <= 0) H.pop_back();
        H.push_back(p);
    }
    return H;
}

#endif
