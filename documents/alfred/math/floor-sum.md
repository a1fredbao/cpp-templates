---
title: Floor Sum (Like-Euclidean)
documentation_of: ./src/alfred/math/floor-sum.hpp
---

## Description

Computes the sum $\sum_{i = 0}^{n - 1} \lfloor \frac{ai + b}{m} \rfloor$ efficiently. It supports both unsigned and signed arguments (with negative values).

## Usage Example

```cpp
#include "floor-sum.hpp"
#include <iostream>

int main() {
    // Computes sum_{i=0}^{3} floor((3i + 2) / 4)
    // i=0: floor(2/4) = 0
    // i=1: floor(5/4) = 1
    // i=2: floor(8/4) = 2
    // i=3: floor(11/4) = 2
    // Sum = 5
    long long ans = floor_sum(3, 2, 4, 4);
    std::cout << "Floor Sum: " << ans << "\n"; // Outputs 5
    return 0;
}
```

## Main Features
- Computes floor sums in logarithmic time, similar to the Euclidean GCD algorithm.
- Supports negative coefficients ($a, b$) by shifting values mapping onto unsigned domain internally.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `floor_sum_unsigned` | Computes the floor sum for non-negative integers | $O(\log m)$ |
| `floor_sum` | Computes the floor sum for generic integers (handles negative inputs) | $O(\log m)$ |

## Notes
- Extremely useful in coordinate geometry, counting lattice points under a line, and number theory.
