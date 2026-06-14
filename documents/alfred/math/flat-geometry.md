---
title: Flat Geometry
documentation_of: ./src/alfred/math/flat-geometry.hpp
---

## Description

Modern C++ 2D computational geometry template, offering Point/Vector structures, basic operations (dot/cross products), polar angle comparator, and 2D convex hull generation via Graham's Scan. It is fully compatible with C++14.

## Usage Example

```cpp
#include "flat-geometry.hpp"
#include <iostream>
#include <vector>

int main() {
    std::vector<Point<Double>> P = {{0, 0}, {2, 0}, {0, 2}, {1, 1}, {2, 2}};
    auto hull = convex_hull(P);
    for (auto &pt : hull) {
        std::cout << pt.x << " " << pt.y << "\n";
    }
    return 0;
}
```

## Main Features
- `Double` helper class to encapsulate floating point comparisons under a configurable epsilon tolerance.
- `Vec<T>` and `Point<T>` representation with full operator overloading.
- Dot product (`dot`) and Cross product (`cross`) operations.
- `PolarAngleComparator` for counterclockwise sorting of points.
- Graham's scan algorithm for computing the convex hull.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `dot` | Computes the dot product of two vectors | $O(1)$ |
| `cross` | Computes the cross product of two vectors | $O(1)$ |
| `convex_hull` | Generates a 2D convex hull from a set of points | $O(N \log N)$ |

## Notes
- Floating point coordinates should use the `Double` helper class as coordinate type `T` to avoid precision issues. Epsilon is configurable via the `AFMT_FLAT_GEOMETRY_EPS` macro (defaults to `1e-9`).
- The returned convex hull starts from the bottom-leftmost point and proceeds counterclockwise. The starting point is repeated at the end of the vector.
