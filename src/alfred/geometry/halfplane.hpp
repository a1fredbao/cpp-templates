#pragma once

#include "polygon.hpp"

#include <algorithm>
#include <cassert>
#include <deque>
#include <type_traits>
#include <vector>

namespace halfplane_detail {

    template <class T>
    Point<T> direction(const Line<T> &l) {
        return l.b - l.a;
    }

    // Polar order in [0, 2pi).
    // No epsilon is used here: the comparator must satisfy strict weak ordering.
    template <class T>
    int angle_half(const Point<T> &v) {
        return (v.y > T(0) || (v.y == T(0) && v.x >= T(0))) ? 0 : 1;
    }

    template <class T>
    bool direction_less(const Point<T> &a, const Point<T> &b) {
        int x = angle_half(a), y = angle_half(b);
        if (x != y) return x < y;
        return cross(a, b) > T(0);
    }

    // Whether two nonzero vectors have exactly the same direction.
    template <class T>
    bool same_direction(const Point<T> &a, const Point<T> &b) {
        return cross(a, b) == T(0) && dot(a, b) > T(0);
    }

    // Signed-side classification with an absolute distance tolerance.
    // Return:
    //   1: strictly inside (left)
    //   0: on the boundary (within eps)
    //  -1: outside (right)
    template <class T>
    int side(const Line<T> &l, const Point<T> &p) {
        Point<T> d = direction(l);
        T v = cross(d, p - l.a);
        T tol = eps<T> * length(d);

        if (v > tol) return 1;
        if (v < -tol) return -1;
        return 0;
    }

    // Intersection of two nonparallel infinite lines.
    // Unlike the general geometry helper, this function does not use epsilon
    // to classify parallel lines: nearly parallel lines are still distinct.
    template <class T>
    bool intersect_lines(
        const Line<T> &a,
        const Line<T> &b,
        Point<T> &p
    ) {
        Point<T> d = direction(a);
        Point<T> e = direction(b);

        T den = cross(d, e);
        if (den == T(0)) return false;

        T t = cross(b.a - a.a, e) / den;
        p = a.a + d * t;
        return true;
    }

    template <class T>
    bool same_point(const Point<T> &a, const Point<T> &b) {
        T e = eps<T>;
        return square(a - b) <= e * e;
    }

} // namespace halfplane_detail

// A half-plane is the closed region to the left of the directed line a -> b.
template <class T>
struct Halfplane : Line<T> {
    using Line<T>::Line;

    Halfplane() = default;

    // Construct the half-plane Ax + By + C >= 0.
    //
    // Use a floating-point coordinate type, e.g. Halfplane<long double>.
    // A = B = 0 is invalid: it describes either the entire plane or
    // the empty set, neither of which is representable by a line.
    template <
        class U = T,
        std::enable_if_t<!std::is_integral_v<U>, int> = 0>
    Halfplane(T A, T B, T C) {
        assert(A != T(0) || B != T(0));
        if (A == T(0) && B == T(0)) return;

        T aa = A < T(0) ? -A : A;
        T ab = B < T(0) ? -B : B;
        T s = std::max(aa, ab);

        // A boundary point.
        if (aa >= ab) {
            this->a = Point<T>(-C / A, T(0));
        } else {
            this->a = Point<T>(T(0), -C / B);
        }

        // The direction (B, -A) makes the feasible region lie on the left.
        // Normalize the direction to avoid unnecessarily large coordinates.
        this->b = this->a + Point<T>(B / s, -A / s);
    }
};

// Intersection of a set of closed half-planes.
//
// Returns a counterclockwise convex polygon if the intersection is
// nonempty, bounded, and has positive area.
//
// Returns an empty polygon if the intersection is:
//   - empty;
//   - unbounded;
//   - a single point or a segment;
//   - invalid (e.g. containing a zero-length boundary line).
//
// Integral input coordinates are promoted to calc_t<T>.
//
// Complexity: O(n log n).
template <class T>
Polygon<calc_t<T>> halfplane_intersection(
    std::vector<Halfplane<T>> hs
) {
    using U = calc_t<T>;
    using HP = Halfplane<U>;

    using halfplane_detail::direction;
    using halfplane_detail::direction_less;
    using halfplane_detail::intersect_lines;
    using halfplane_detail::same_direction;
    using halfplane_detail::same_point;
    using halfplane_detail::side;

    std::vector<HP> ls;
    ls.reserve(hs.size());

    // Convert to calculation precision and reject degenerate lines.
    for (const auto &h : hs) {
        Point<U> a(h.a), b(h.b);
        if (a == b) return {};
        ls.emplace_back(a, b);
    }

    if (ls.size() < 3) return {};

    // 1. Sort by polar angle.
    std::sort(ls.begin(), ls.end(), [](const HP &a, const HP &b) {
        return direction_less(direction(a), direction(b));
    });

    // 2. Merge parallel half-planes with the same direction.
    // For the same direction, the larger cross(d, a) gives the
    // more restrictive half-plane.
    std::vector<HP> unique;
    unique.reserve(ls.size());

    for (const auto &h : ls) {
        if (!unique.empty() &&
            same_direction(direction(unique.back()), direction(h))) {
            Point<U> d = direction(unique.back());

            U offset = cross(d, h.a - unique.back().a);
            U tol = eps<U> * length(d);

            if (offset > tol) unique.back() = h;
        } else {
            unique.push_back(h);
        }
    }

    ls.swap(unique);

    int n = ls.size();
    if (n < 3) return {};

    // 3. Check boundedness.
    //
    // A bounded intersection requires every circular angular gap
    // between consecutive boundary directions to be strictly < pi.
    //
    // After removing duplicate directions, this is equivalent to
    // cross(d[i], d[i + 1]) > 0 for every cyclic adjacent pair.
    for (int i = 0; i < n; ++i) {
        Point<U> a = direction(ls[i]);
        Point<U> b = direction(ls[(i + 1) % n]);

        if (cross(a, b) <= U(0)) return {};
    }

    // Relation between the intersection of a, b and half-plane h:
    //   0: inside or on the boundary of h;
    //   1: outside h;
    //   2: a and b are parallel, so there is no finite intersection.
    auto relation = [&](const HP &a, const HP &b, const HP &h) -> int {
        Point<U> p;
        if (!intersect_lines(a, b, p)) return 2;
        return side(h, p) < 0 ? 1 : 0;
    };

    // 4. Half-plane intersection using a deque.
    std::deque<HP> q;

    for (const auto &h : ls) {
        // Remove invalid intersections at the back.
        while (q.size() > 1) {
            int r = relation(q[q.size() - 2], q.back(), h);

            if (r == 2) return {};
            if (r == 0) break;

            q.pop_back();
        }

        // Remove invalid intersections at the front.
        while (q.size() > 1) {
            int r = relation(q[0], q[1], h);

            if (r == 2) return {};
            if (r == 0) break;

            q.pop_front();
        }

        // Same-direction constraints have already been merged.
        // Opposite parallel boundaries cannot provide a finite vertex.
        if (!q.empty() &&
            cross(direction(q.back()), direction(h)) == U(0)) {
            return {};
        }

        q.push_back(h);
    }

    // 5. Close the deque cyclically.
    while (q.size() > 2) {
        int r = relation(q[q.size() - 2], q.back(), q.front());

        if (r == 2) return {};
        if (r == 0) break;

        q.pop_back();
    }

    while (q.size() > 2) {
        int r = relation(q[0], q[1], q.back());

        if (r == 2) return {};
        if (r == 0) break;

        q.pop_front();
    }

    if (q.size() < 3) return {};

    // 6. Construct polygon vertices.
    Polygon<U> poly;
    poly.reserve(q.size());

    for (int i = 0; i < (int)q.size(); ++i) {
        Point<U> p;

        if (!intersect_lines(q[i], q[(i + 1) % q.size()], p)) {
            return {};
        }

        if (poly.empty() || !same_point(poly.back(), p)) {
            poly.push_back(p);
        }
    }

    if (poly.size() > 1 &&
        same_point(poly.front(), poly.back())) {
        poly.pop_back();
    }

    if (poly.size() < 3) return {};

    // 7. Reject degenerate polygons.
    // The vertices are expected to be counterclockwise.
    U twice_area = 0;
    U perimeter = 0;

    for (int i = 1; i + 1 < (int)poly.size(); ++i) {
        twice_area += cross(
            poly[i] - poly[0],
            poly[i + 1] - poly[0]
        );
    }

    for (int i = 0; i < (int)poly.size(); ++i) {
        perimeter += length(
            poly[(i + 1) % poly.size()] - poly[i]
        );
    }

    if (twice_area <= eps<U> * perimeter) return {};

    return poly;
}
