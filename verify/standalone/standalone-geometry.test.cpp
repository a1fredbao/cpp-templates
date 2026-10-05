// competitive-verifier: STANDALONE

#include "../../src/alfred/geometry/all.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>

static bool close(long double a, long double b, long double tol = 1e-8) {
	return std::abs(a - b) <=
	       tol * std::max<long double>({1, std::abs(a), std::abs(b)});
}

static long double brute_rectangle_area(const Polygon<long long> &p) {
	long double ans = 1e100;
	for (int i = 0, n = p.size(); i < n; i++) {
		Point<long double> u = normalize(p[(i + 1) % n] - p[i]);
		Point<long double> v = rotate_90(u);
		long double lox = 1e100, hix = -1e100, loy = 1e100, hiy = -1e100;
		for (const auto &q : p) {
			Point<long double> x(q);
			lox = std::min(lox, dot(x, u));
			hix = std::max(hix, dot(x, u));
			loy = std::min(loy, dot(x, v));
			hiy = std::max(hiy, dot(x, v));
		}
		ans = std::min(ans, (hix - lox) * (hiy - loy));
	}
	return ans;
}

static long double brute_rectangle_perimeter(const Polygon<long long> &p) {
	long double ans = 1e100;
	for (int i = 0, n = p.size(); i < n; i++) {
		Point<long double> u = normalize(p[(i + 1) % n] - p[i]);
		Point<long double> v = rotate_90(u);
		long double lox = 1e100, hix = -1e100, loy = 1e100, hiy = -1e100;
		for (const auto &q : p) {
			Point<long double> x(q);
			lox = std::min(lox, dot(x, u));
			hix = std::max(hix, dot(x, u));
			loy = std::min(loy, dot(x, v));
			hiy = std::max(hiy, dot(x, v));
		}
		ans = std::min(ans, 2 * ((hix - lox) + (hiy - loy)));
	}
	return ans;
}

static long double brute_diameter(const Polygon<long long> &p) {
	long double ans = 0;
	for (int i = 0; i < int(p.size()); i++) {
		for (int j = i + 1; j < int(p.size()); j++) {
			ans = std::max(ans, std::sqrt((long double)square(p[i] - p[j])));
		}
	}
	return ans;
}

static long double brute_triangle_area(const Polygon<long long> &p) {
	long double ans = 0;
	for (int i = 0; i < int(p.size()); i++) {
		for (int j = i + 1; j < int(p.size()); j++) {
			for (int k = j + 1; k < int(p.size()); k++) {
				auto c = cross(p[i], p[j], p[k]);
				if (c < 0) c = -c;
				ans = std::max(ans, (long double)c / 2);
			}
		}
	}
	return ans;
}

static long double
brute_min_distance(const Polygon<long long> &a, const Polygon<long long> &b) {
	for (int i = 0; i < int(a.size()); i++) {
		for (int j = 0; j < int(b.size()); j++) {
			if (segment_intersection_check(
			        Line<long long>(a[i], a[(i + 1) % a.size()]),
			        Line<long long>(b[j], b[(j + 1) % b.size()])
			    ) != 0) {
				return 0;
			}
		}
	}
	long double ans = 1e100;
	for (int i = 0; i < int(a.size()); i++) {
		for (int j = 0; j < int(b.size()); j++) {
			ans = std::min(
			    ans, dist_point_segment(
			             a[i], Line<long long>(b[j], b[(j + 1) % b.size()])
			         )
			);
			ans = std::min(
			    ans, dist_point_segment(
			             b[j], Line<long long>(a[i], a[(i + 1) % a.size()])
			         )
			);
		}
	}
	return ans;
}

static Circle<long double> brute_min_circle(const Polygon<long long> &p) {
	Circle<long double> best{
	    {0, 0},
        1e100
	};
	auto consider = [&](Circle<long double> c) {
		if (c.r < 0) return;
		for (const auto &q : p) {
			if (!inside_circle(Point<long double>(q), c)) return;
		}
		if (c.r < best.r) best = c;
	};
	for (const auto &a : p)
		consider({
		    {(long double)a.x, (long double)a.y},
            0
		});
	for (int i = 0; i < int(p.size()); i++) {
		for (int j = i + 1; j < int(p.size()); j++) {
			Point<long double> a(p[i]), b(p[j]);
			consider({(a + b) / 2, dist(a, b) / 2});
			for (int k = j + 1; k < int(p.size()); k++) {
				consider(circumcircle(a, b, Point<long double>(p[k])));
			}
		}
	}
	return best;
}

int main() {
	auto old_eps = eps<double>;
	eps<double> = 1e-7;
	assert(sgn(5e-8) == 0);
	assert(sgn(2e-7) > 0);
	eps<double> = old_eps;

	Polygon<long long> points{
	    {0, 0},
        {2, 0},
        {2, 2},
        {0, 2},
        {1, 0},
        {0, 1},
        {1, 1}
	};
	auto hull = convex_hull(points);
	assert(hull.size() == 4);
	auto hull_area = area(hull);
	assert((hull_area < 0 ? -hull_area : hull_area) == 8);
	auto hull_with_collinear = convex_hull(points, true);
	assert(hull_with_collinear.size() == 6);

	assert(point_in_polygon(Point<long long>(1, 1), hull) == 1);
	assert(point_in_polygon(Point<long long>(1, 0), hull) == 2);
	assert(point_in_polygon(Point<long long>(3, 3), hull) == 0);
	assert(point_in_convex(Point<long long>(1, 1), hull) == 1);
	assert(point_in_convex(Point<long long>(0, 1), hull) == 2);

	auto [diameter, da, db] = convex_diameter(hull);
	assert(close(diameter, std::sqrt(8.0L)));
	assert(square(da - db) == 8);

	auto rect = min_bounding_rectangle(hull);
	assert(close(rect.area, 4));
	assert(close(rect.perimeter, 8));

	assert(
	    segment_intersection_check(
	        Line<long long>({
	            {0, 0},
                {2, 2}
	}),
	        Line<long long>({{0, 2}, {2, 0}})
	    ) == 1
	);
	assert(
	    segment_intersection_check(
	        Line<long long>({
	            {0, 0},
                {2, 2}
	}),
	        Line<long long>({{2, 2}, {3, 3}})
	    ) == 3
	);
	assert(segment_in_polygon(
	    Line<long long>({
	        {0, 0},
            {2, 2}
	}),
	    hull
	));
	assert(!segment_in_polygon(
	    Line<long long>({
	        {-1, 1},
            { 3, 1}
	}),
	    hull
	));

	std::vector<Halfplane<long long>> halfplanes{
	    {{0, 0}, {0, 1}},
	    {{0, 0}, {1, 0}},
	    {{1, 0}, {0, 1}},
	};
	auto triangle = halfplane_intersection(halfplanes);
	assert(triangle.size() == 3);
	assert(close(std::abs(area(triangle)), 1));
	std::vector<Halfplane<long long>> square_halfplanes{
	    {{0, 0}, {1, 0}},
	    {{1, 0}, {1, 1}},
	    {{1, 1}, {0, 1}},
	    {{0, 1}, {0, 0}},
	};
	auto clipped_square = halfplane_intersection(square_halfplanes);
	assert(clipped_square.size() == 4);
	assert(close(std::abs(area(clipped_square)), 2));

	Polygon<long long> unit_square{
	    {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
	};
	auto minkowski = minkowski_sum(unit_square, unit_square);
	assert(minkowski.size() == 4);
	auto minkowski_area = area(minkowski);
	assert((minkowski_area < 0 ? -minkowski_area : minkowski_area) == 8);
	Polygon<long long> shifted{
	    {3, 0},
        {4, 0},
        {4, 1},
        {3, 1}
	};
	assert(close(min_dist_convex(unit_square, shifted), 2));

	Circle<long long> unit({
	    {0, 0},
        1
	});
	auto line_intersections = circle_line_intersection(
	    unit, Line<long long>({
	              {-2, 0},
                  { 2, 0}
	})
	);
	assert(line_intersections.size() == 2);
	Circle<long long> other({
	    {4, 0},
        1
	});
	assert(circle_circle_intersection(unit, other).empty());
	assert(common_tangents(unit, other).size() == 4);
	auto two_circles = circle_circle_intersection(
	    unit, Circle<long long>({
	              {1, 0},
                  1
	})
	);
	assert(two_circles.size() == 2);
	assert(close(two_circles[0].x, 0.5));
	assert(close(std::abs(two_circles[0].y), std::sqrt(3.0L) / 2));
	assert(tangent_points(Point<long long>(2, 0), unit).size() == 2);

	Polygon<long long> circle_points{
	    {0, 0},
        {2, 0},
        {2, 2},
        {0, 2}
	};
	Circle<long double> cover = minimum_enclosing_circle(circle_points);
	assert(close(cover.p.x, 1) && close(cover.p.y, 1));
	assert(close(cover.r, std::sqrt(2.0L)));

	Polygon<long long> cut_square{
	    {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
	};
	auto cut = cut_polygon(
	    cut_square, Line<long long>({
	                    {0, 0},
                        {1, 1}
	})
	);
	assert(cut.size() == 3);
	assert(close(std::abs(area(cut)), 1));

	std::mt19937 rng(20261005);
	for (int iteration = 0; iteration < 200; iteration++) {
		Polygon<long long> points;
		for (int i = 0; i < 20; i++) {
			points.push_back({int(rng() % 21) - 10, int(rng() % 21) - 10});
		}
		auto convex = convex_hull(points);
		if (convex.size() < 3) continue;
		assert(is_convex(convex));
		for (int x = -10; x <= 10; x++) {
			for (int y = -10; y <= 10; y++) {
				Point<long long> q(x, y);
				assert(
				    point_in_convex(q, convex) == point_in_polygon(q, convex)
				);
			}
		}
		auto [diameter, p, q] = convex_diameter(convex);
		long double brute_d = brute_diameter(convex);
		assert(close(diameter, brute_d, 1e-7));
		assert(close((long double)square(p - q), brute_d * brute_d, 1e-7));
		assert(
		    close(max_triangle_area(convex), brute_triangle_area(convex), 1e-7)
		);
		auto r = min_bounding_rectangle(convex);
		long double brute_area = brute_rectangle_area(convex);
		if (!close(r.area, brute_area, 1e-7)) {
			std::cerr << "area mismatch: " << r.area << ' ' << brute_area
			          << '\n';
			for (auto q : convex) std::cerr << q << ' ';
			std::cerr << '\n';
			assert(false);
		}
		long double brute_perimeter = brute_rectangle_perimeter(convex);
		if (!close(r.perimeter, brute_perimeter, 1e-7)) {
			std::cerr << "perimeter mismatch: " << r.perimeter << ' '
			          << brute_perimeter << '\n';
			for (auto q : convex) std::cerr << q << ' ';
			std::cerr << '\n';
			assert(false);
		}
	}

	for (int iteration = 0; iteration < 100; iteration++) {
		Polygon<long long> a, b;
		for (int i = 0; i < 8; i++) {
			a.push_back({int(rng() % 11) - 5, int(rng() % 11) - 5});
			b.push_back({int(rng() % 11) + 8, int(rng() % 11) - 5});
		}
		auto ca = convex_hull(a);
		auto cb = convex_hull(b);
		if (ca.size() < 3 || cb.size() < 3) continue;
		auto sum = minkowski_sum(ca, cb);
		Polygon<long long> expected;
		for (const auto &x : ca) {
			for (const auto &y : cb) expected.push_back(x + y);
		}
		expected = convex_hull(expected);
		assert(sum.size() == expected.size());
		for (const auto &x : sum) assert(point_in_convex(x, expected) == 2);
		assert(
		    close(min_dist_convex(ca, cb), brute_min_distance(ca, cb), 1e-7)
		);
	}

	for (int iteration = 0; iteration < 100; iteration++) {
		Polygon<long long> points;
		for (int i = 0; i < 8; i++) {
			points.push_back({int(rng() % 21) - 10, int(rng() % 21) - 10});
		}
		Circle<long double> got = minimum_enclosing_circle(points);
		Circle<long double> expected = brute_min_circle(points);
		assert(close(got.r, expected.r, 1e-7));
		for (const auto &p : points) {
			assert(inside_circle(Point<long double>(p), got));
		}
	}
	return 0;
}
