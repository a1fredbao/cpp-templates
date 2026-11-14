#pragma once

#include "polygon.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>

namespace circle_detail {

    template <class T>
    using U = calc_t<T>;

    template <class T>
    bool zero(T x, T scale = T(1)) {
        if constexpr (std::is_integral_v<T>) {
            return x == T(0);
        } else {
            return std::abs(x) <= eps<T> * std::max(T(1), std::abs(scale));
        }
    }

    template <class T>
    bool equal(T a, T b, T scale = T(1)) {
        return zero(a - b, scale);
    }

    template <class T>
    bool positive(T x, T scale = T(1)) {
        if constexpr (std::is_integral_v<T>) {
            return x > T(0);
        } else {
            return x > eps<T> * std::max(T(1), std::abs(scale));
        }
    }

    template <class T>
    bool negative(T x, T scale = T(1)) {
        if constexpr (std::is_integral_v<T>) {
            return x < T(0);
        } else {
            return x < -eps<T> * std::max(T(1), std::abs(scale));
        }
    }

    template <class T>
    T sq(T x) {
        return x * x;
    }

    template <class T>
    T norm2(const Point<T> &p) {
        return p.x * p.x + p.y * p.y;
    }

    template <class T>
    Point<T> perpendicular(const Point<T> &p) {
        return Point<T>(-p.y, p.x);
    }

    template <class T>
    bool same_point(const Point<T> &a, const Point<T> &b) {
        Point<T> d = a - b;
        T s = std::max({T(1), std::abs(a.x), std::abs(a.y), std::abs(b.x), std::abs(b.y)});
        return zero(norm2(d), s * s);
    }

} // namespace circle_detail

// Circle with center p and nonnegative radius r.
//
// r < 0 is not a valid geometric circle.
template <class T>
struct Circle {
    Point<T> p{};
    T r{};

    Circle() = default;

    Circle(Point<T> p_, T r_) : p(p_), r(r_) {
        assert(r >= T(0));
    }
};

// Whether point p belongs to the closed disk.
//
// Boundary points are considered inside.
template <class T>
bool inside_circle(
    const Point<T> &p,
    const Circle<T> &c
) {
    using U = calc_t<T>;

    Point<U> d = Point<U>(p) - Point<U>(c.p);
    U d2 = circle_detail::norm2(d);
    U r = U(c.r);

    U scale = std::max<U>(U(1), r * r);

    if (d2 <= r * r) return true;

    if constexpr (std::is_integral_v<U>) {
        return false;
    } else {
        return d2 <= r * r + U(32) * eps<U> * scale;
    }
}

// Intersection of an infinite line and a circle.
//
// Return:
//   0 points: no intersection
//   1 point : tangent
//   2 points: proper intersection
//
// A zero-length line is interpreted as a single point.
template <class T>
std::vector<Point<calc_t<T>>>
circle_line_intersection(
    const Circle<T> &c,
    const Line<T> &l
) {
    using U = calc_t<T>;

    Point<U> a(l.a), b(l.b), o(c.p);
    Point<U> d = b - a;

    U d2 = circle_detail::norm2(d);
    U r = U(c.r);
    U r2 = r * r;

    // Degenerate line: it is just one point.
    if (circle_detail::zero(d2, std::max<U>(U(1), r2))) {
        U x2 = circle_detail::norm2(a - o);
        U scale = std::max<U>(U(1), r2);

        if (circle_detail::equal(x2, r2, scale))
            return {a};

        return {};
    }

    // Projection of the center onto the line.
    U t = dot(o - a, d) / d2;
    Point<U> q = a + d * t;

    // h^2 is the squared distance from the projection to either
    // intersection point.
    U h2 = r2 - circle_detail::norm2(q - o);

    U scale = std::max({U(1), r2, circle_detail::norm2(q - o)});

    U tol = U(32) * eps<U> * scale;

    if (h2 < -tol)
        return {};

    if (std::abs(h2) <= tol)
        return {q};

    // Normalize the line direction.
    U len = std::sqrt(d2);
    Point<U> dir = d / len;

    U h = std::sqrt(std::max<U>(U(0), h2));

    Point<U> p1 = q - dir * h;
    Point<U> p2 = q + dir * h;

    return {p1, p2};
}

// Intersection of a closed segment and a circle.
//
// This is implemented by solving the infinite-line intersection first,
// then filtering by the segment parameter.  Using the parameter instead
// of point_on_segment avoids another independent geometric tolerance.
template <class T>
std::vector<Point<calc_t<T>>>
circle_segment_intersection(
    const Circle<T> &c,
    const Line<T> &s
) {
    using U = calc_t<T>;

    Point<U> a(s.a), b(s.b);
    Point<U> d = b - a;

    U d2 = circle_detail::norm2(d);

    if (circle_detail::zero(d2))
        return circle_line_intersection(c, s);

    std::vector<Point<U>> res;

    for (const auto &p : circle_line_intersection(c, s)) {
        U t = dot(p - a, d) / d2;

        U tol = U(32) * eps<U>;

        if (t >= -tol && t <= U(1) + tol) {
            res.push_back(p);
        }
    }

    return res;
}

// Intersection of two circles.
//
// Return:
//   0 points: disjoint / one strictly contains the other
//   1 point : tangent
//   2 points: proper intersection
//
// Coincident circles have infinitely many common points and return {}.
template <class T>
std::vector<Point<calc_t<T>>>
circle_circle_intersection(
    const Circle<T> &c1,
    const Circle<T> &c2
) {
    using U = calc_t<T>;

    Point<U> a(c1.p), b(c2.p);
    Point<U> v = b - a;

    U r1 = U(c1.r);
    U r2 = U(c2.r);

    U d2 = circle_detail::norm2(v);
    U d = std::sqrt(d2);

    U scale = std::max({U(1), r1, r2, d});

    U tol = U(32) * eps<U> * scale;

    // Concentric circles.
    if (d <= tol) {
        // Coincident circles: infinitely many intersections.
        if (std::abs(r1 - r2) <= tol)
            return {};

        // Concentric with different radii: no intersection.
        return {};
    }

    // No intersection if the circles are too far apart.
    if (d > r1 + r2 + tol)
        return {};

    // No intersection if one circle is strictly inside the other.
    if (d < std::abs(r1 - r2) - tol)
        return {};

    // Distance from c1 to the radical chord along c1 -> c2.
    U x = (r1 * r1 - r2 * r2 + d2) / (U(2) * d);

    // h^2 = distance from chord center to either intersection squared.
    U h2 = r1 * r1 - x * x;

    U scale2 = std::max({U(1), r1 * r1, r2 * r2, d2});

    U tol2 = U(64) * eps<U> * scale2;

    Point<U> o = a + v * (x / d);

    // Tangency.
    if (h2 <= tol2)
        return {o};

    if (h2 < U(0))
        return {};

    U h = std::sqrt(h2);

    Point<U> n = circle_detail::perpendicular(v) / d;

    Point<U> p1 = o + n * h;
    Point<U> p2 = o - n * h;

    return {p1, p2};
}

// Tangent points from p to circle c.
//
// Return:
//   0 points: p strictly inside circle
//   1 point : p lies on circle
//   2 points: p is outside circle
//
// A zero-radius circle has no ordinary tangent line, so it returns {}.
template <class T>
std::vector<Point<calc_t<T>>>
tangent_points(
    const Point<T> &p,
    const Circle<T> &c
) {
    using U = calc_t<T>;

    Point<U> q(p), o(c.p);
    Point<U> v = q - o;

    U d2 = circle_detail::norm2(v);
    U r = U(c.r);
    U r2 = r * r;

    if (circle_detail::zero(r))
        return {};

    U scale = std::max({U(1), r2, d2});

    U tol = U(32) * eps<U> * scale;

    // p is strictly inside.
    if (d2 < r2 - tol)
        return {};

    // p is on the circle.
    if (std::abs(d2 - r2) <= tol)
        return {q};

    // Foot of the perpendicular from center to the tangent chord.
    Point<U> h = o + v * (r2 / d2);

    // Distance from h to either tangent point.
    U t2 = (d2 - r2) * r2 / d2;

    if (t2 <= tol)
        return {h};

    U t = std::sqrt(t2);

    Point<U> n = circle_detail::perpendicular(v) / std::sqrt(d2);

    return {
        h + n * t,
        h - n * t
    };
}

// Common tangent lines of two circles.
//
// Up to four tangent lines are returned:
//   2 external tangents
//   2 internal tangents
//
// Coincident circles: {}
// One circle strictly inside another: only external tangents may exist.
// Internally tangent circles: one internal tangent.
// Externally tangent circles: one internal tangent + two external tangents.
//
// A zero-radius circle is supported as a point-circle.
template <class T>
std::vector<Line<calc_t<T>>>
common_tangents(
    const Circle<T> &c1,
    const Circle<T> &c2
) {
    using U = calc_t<T>;

    Point<U> a(c1.p), b(c2.p);
    Point<U> v = b - a;

    U r1 = U(c1.r);
    U r2 = U(c2.r);

    U d2 = circle_detail::norm2(v);
    U d = std::sqrt(d2);

    U scale = std::max({U(1), r1, r2, d});

    U tol = U(32) * eps<U> * scale;

    // Coincident centers.
    if (d <= tol) {
        // Equal circles: infinitely many common tangents.
        if (std::abs(r1 - r2) <= tol)
            return {};

        // Concentric unequal circles: no common tangent.
        return {};
    }

    std::vector<Line<U>> res;

    Point<U> e = v / d;
    Point<U> n = circle_detail::perpendicular(e);

    // For a tangent line with unit normal m:
    //
    //     m · x = k
    //
    // and oriented tangent points
    //
    //     p1 = a + m * r1
    //     p2 = b + m * r2
    //
    // the two tangent points lie on the same line iff
    //
    //     m · (b-a) = r1 - r2.
    //
    // Thus for external tangents:
    //
    //     cos(theta) = (r1-r2)/d.
    //
    // For internal tangents:
    //
    //     cos(theta) = (r1+r2)/d.
    auto add_family = [&](U rr, bool internal) {
        U x = rr / d;

        U xscale = std::max<U>(U(1), std::abs(x));
        U xtol = U(32) * eps<U> * xscale;

        if (x > U(1) + xtol || x < -U(1) - xtol)
            return;

        x = std::max<U>(U(-1), std::min<U>(U(1), x));

        U y2 = U(1) - x * x;

        if (y2 < U(0)) {
            if (y2 < -U(64) * eps<U>)
                return;
            y2 = U(0);
        }

        U y = std::sqrt(y2);

        for (int s : {-1, 1}) {
            U sy = U(s) * y;

            // Unit normal.
            Point<U> m = e * x + n * sy;

            Point<U> p1 = a + m * r1;
            Point<U> p2;

            if (internal)
                p2 = b - m * r2;
            else
                p2 = b + m * r2;

            if (circle_detail::same_point(p1, p2))
                continue;

            res.emplace_back(p1, p2);
        }
    };

    // External tangents.
    add_family(r1 - r2, false);

    // Internal tangents.
    add_family(r1 + r2, true);

    // Remove accidental duplicates caused by tangency / tolerance.
    std::vector<Line<U>> ans;

    auto same_line = [&](const Line<U> &x, const Line<U> &y) {
        Point<U> dx = x.b - x.a;
        Point<U> dy = y.b - y.a;

        if (circle_detail::zero(
                cross(dx, dy),
                std::max<U>(U(1), length(dx) * length(dy))
            )) {
            return circle_detail::zero(
                cross(dx, y.a - x.a),
                std::max<U>(U(1), length(dx) * length(y.a - x.a))
            );
        }

        return false;
    };

    for (const auto &l : res) {
        bool duplicate = false;

        for (const auto &x : ans) {
            if (same_line(l, x)) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            ans.push_back(l);
    }

    return ans;
}

// Circumcenter of three points.
//
// Returns the default point when the triangle is degenerate.
template <class T>
Point<calc_t<T>>
circumcenter(
    const Point<T> &a,
    const Point<T> &b,
    const Point<T> &c
) {
    using U = calc_t<T>;

    Point<U> x(a), y(b), z(c);

    Point<U> u = y - x;
    Point<U> v = z - x;

    U d = U(2) * cross(u, v);

    U scale = std::max({U(1), length(u) * length(v)});

    if (circle_detail::zero(d, scale))
        return {};

    U uu = circle_detail::norm2(u);
    U vv = circle_detail::norm2(v);

    // Solve:
    //
    //     o · u = |u|² / 2
    //     o · v = |v|² / 2
    //
    // in coordinates relative to a.
    U ox = (uu * v.y - vv * u.y) / d;
    U oy = (u.x * vv - v.x * uu) / d;

    return x + Point<U>(ox, oy);
}

// Circumcircle of three points.
//
// If the points are collinear, returns an invalid circle with r = -1.
//
// This function deliberately does not use Circle's constructor for the
// invalid sentinel.
template <class T>
Circle<calc_t<T>>
circumcircle(
    const Point<T> &a,
    const Point<T> &b,
    const Point<T> &c
) {
    using U = calc_t<T>;

    Point<U> x(a), y(b), z(c);
    Point<U> u = y - x;
    Point<U> v = z - x;

    U cr = cross(u, v);

    U scale = std::max({U(1), length(u) * length(v)});

    if (circle_detail::zero(cr, scale)) {
        Circle<U> res;
        res.p = {};
        res.r = U(-1);
        return res;
    }

    Point<U> o = circumcenter(a, b, c);

    return Circle<U>(o, dist(o, x));
}

// Incenter of a nondegenerate triangle.
template <class T>
Point<calc_t<T>>
incenter(
    const Point<T> &a,
    const Point<T> &b,
    const Point<T> &c
) {
    using U = calc_t<T>;

    U x = dist(b, c);
    U y = dist(c, a);
    U z = dist(a, b);

    U s = x + y + z;

    assert(!circle_detail::zero(s));

    return Point<U>(
        (x * U(a.x) + y * U(b.x) + z * U(c.x)) / s,
        (x * U(a.y) + y * U(b.y) + z * U(c.y)) / s
    );
}

// Minimum enclosing circle.
//
// Randomized incremental construction.
//
// Empty input returns Circle({}, 0).
//
// The result is deterministic because the shuffle seed is fixed.
// Expected complexity is O(n).
template <class T>
Circle<calc_t<T>>
minimum_enclosing_circle(
    Polygon<T> points
) {
    using U = calc_t<T>;

    if (points.empty())
        return Circle<U>({}, U(0));

    std::mt19937_64 rng(712367821);

    std::shuffle(points.begin(), points.end(), rng);

    Polygon<U> p;
    p.reserve(points.size());

    for (const auto &x : points)
        p.emplace_back(x);

    auto make1 = [&](const Point<U> &a) {
        return Circle<U>(a, U(0));
    };

    auto make2 = [&](const Point<U> &a, const Point<U> &b) {
        Point<U> o = (a + b) / U(2);
        return Circle<U>(o, dist(a, b) / U(2));
    };

    auto make3 = [&](const Point<U> &a,
                     const Point<U> &b,
                     const Point<U> &c) {
        Circle<U> cc = circumcircle(a, b, c);

        if (cc.r >= U(0))
            return cc;

        // Collinear: the minimum circle is determined by the
        // two farthest points.
        U ab = circle_detail::norm2(a - b);
        U bc = circle_detail::norm2(b - c);
        U ca = circle_detail::norm2(c - a);

        if (ab >= bc && ab >= ca)
            return make2(a, b);

        if (bc >= ab && bc >= ca)
            return make2(b, c);

        return make2(c, a);
    };

    Circle<U> c = make1(p[0]);

    for (int i = 1; i < (int)p.size(); ++i) {
        if (inside_circle(p[i], c))
            continue;

        c = make1(p[i]);

        for (int j = 0; j < i; ++j) {
            if (inside_circle(p[j], c))
                continue;

            c = make2(p[i], p[j]);

            for (int k = 0; k < j; ++k) {
                if (inside_circle(p[k], c))
                    continue;

                c = make3(p[i], p[j], p[k]);
            }
        }
    }

    return c;
}