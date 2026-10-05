# Geometry

## 1. Core Architecture & Type System

### Single Unified `Point<T>` Structure

* **Avoid Rigid Type Separation (`Point` vs `Vec`)**: While distinguishing affine points and vectors is mathematically strict, using separate classes with `explicit` constructors creates high type-casting friction during timed contests (e.g., in polar sorting, convex hull algorithms, or line intersections).
* **Position Vector Strategy**: Treat points as vectors from the origin ($O \to P$). Use a single `Point<T>` template struct and provide semantic alias `using Vec = Point<T>;` for code readability.

### Two-Layer Functional Paradigm

Separate all geometric operations into two distinct layers:

1. **Exact Topological Predicates Layer (0-Error Layer)**

* **Allowed Operations**: Addition, subtraction, multiplication ($+$, $-$, $*$ via `dot` and `cross`).
* **Scope**: Orientations (`to_left`), non-strict/strict cross tests, convex hull, polar sorting, point-on-segment checks.
* **Guarantee**: Zero precision loss when operating on integral types (`long long`, `__int128_t`).

1. **Metric & Construction Layer (Floating-Point Layer)**

* **Allowed Operations**: Division, square roots, trigonometric functions ($/$, `std::sqrt`, `std::atan2`, `std::acos`).
* **Scope**: Line intersections, projections, distances, unit normalization, circumcenters, circle operations.
* **Guarantee**: Bounded floating-point error governed by $\epsilon$.

---

## 2. Precision & Epsilon ($\epsilon$) Handling Strategy

### Unified Sign & Comparison Helpers

Never compare floating-point quantities directly with `==`, `<`, or `>`. Route all comparison queries through unified `sgn` and `cmp` helpers that adapt to the underlying type via `constexpr` or template specialization:

```cpp
template <typename T>
T eps = 1e-9;

template <> inline constexpr double eps<double> = 1e-9;
template <> inline constexpr long double eps<long double> = 1e-12;
template <> inline constexpr int eps<int> = 0;
template <> inline constexpr long long eps<long long> = 0;

template <typename T>
constexpr int sgn(T x) {
    if constexpr (std::is_integral_v<T>) {
        return (x > 0) - (x < 0);
    } else {
        if (std::abs(x) <= eps<T>) return 0;
        return (x > 0) - (x < 0);
    }
}

template <typename T>
constexpr int cmp(T a, T b) {
    return sgn(a - b);
}

```

### Dimensionality Alignment ($[L]$ vs $[L^2]$)

Floating-point zero-thresholds must match the physical quantity's dimension:

* **Linear Quantity $[L]$** (e.g., distance, point-to-line offset): Compare against $\epsilon$.
* **Quadratic Quantity $[L^2]$** (e.g., squared distance, dot product, cross product): Compare against $\epsilon^2$ or scale by vector magnitude.
* **Golden Rule**: To prevent false equality or premature underflow, prefer reducing comparisons back to $1\text{D}$ linear quantities (e.g., `sgn(length(p))` instead of `sgn(square(p))`).

### Magnitude Scaling for Large Coordinates

When floating-point coordinates have a maximum scale $M$, the absolute error of cross/dot products scales proportional to $M \cdot \epsilon$. Adjust parallel/collinear checks accordingly:

```cpp
template <typename T>
bool parallel(const Point<T>& a, const Point<T>& b) {
    T scale = length(a) * length(b);
    return std::abs(cross(a, b)) <= eps<T> * std::max(T(1), scale);
}

```

---

## 3. Exact Integer Algorithms (Zero-Floating-Point Precision)

### Trigonometry-Free Polar Angle Sorting

Avoid `std::atan2` when coordinates are integers. Divide points by half-planes (quadrants) and order them using exact cross products:

```cpp
template <class T>
int quad(const Point<T> &p) {
    if (p.x == 0 && p.y == 0) return 0;
    if (p.y > 0 || (p.y == 0 && p.x > 0)) return 1; // Upper half-plane
    return -1;                                     // Lower half-plane
}

template <class T>
bool polar_cmp(const Point<T> &a, const Point<T> &b) {
    int qa = quad(a), qb = quad(b);
    if (qa != qb) return qa < qb;
    return sgn(cross(a, b)) > 0;
}

```

### Deferred Floating-Point Conversion

Keep input coordinates in `Point<long long>` throughout predicate execution (e.g., Andrew's Convex Hull). Cast to `Point<double>` or `Point<long double>` **only** when metric constructions (e.g., ray intersections) are required.

---

## 4. Summary of Design Choices

| Dimension           | Recommended Approach                                    | Rejected Alternative                  | Reason                                                                                       |
| ------------------- | ------------------------------------------------------- | ------------------------------------- | -------------------------------------------------------------------------------------------- |
| **Type Design**     | Single `Point<T>` for points & vectors                  | Explicit `Point` / `Vec` class split  | Eliminates constant type-casting and verbose compiler errors under contest pressure.         |
| **Epsilon Control** | Templated `eps<T>` variable                             | Global `#define EPS` or `constexpr`   | Supports switching between `double` and `long double`, as well as runtime scale adjustments. |
| **Comparisons**     | Unified `sgn()` and `cmp()` helpers                     | Raw operators (`==`, `<`, `>`)        | Prevents precision leaks and subtle floating-point inequality bugs.                          |
| **Polar Sorting**   | Quadrant splitting + Cross product                      | `std::atan2`                          | Guarantees exact $O(1)$ sorting without floating-point precision degradation.                |
| **Dimension Rules** | Convert back to $1\text{D}$ ($[L]$) or use $\epsilon^2$ | Applying $\epsilon$ to squared values | Prevents false positives when comparing quadratic values like squared distances.             |
