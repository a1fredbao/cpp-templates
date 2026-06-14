---
title: Radix Sort
documentation_of: ./src/alfred/algorithm/radix-sort.hpp
---

## Description

A fast, non-comparative sorting algorithm that sorts integers by grouping keys by individual digits. It is optimized using bitwise shifts.

## Usage Example

```cpp
#include "radix-sort.hpp"
#include <iostream>

int main() {
    unsigned int a[] = {170, 45, 75, 90, 802, 24, 2, 66};
    radix_sort(a, 8);
    for (int i = 0; i < 8; i++) {
        std::cout << a[i] << " ";
    }
    std::cout << "\n";
    return 0;
}
```

## Main Features
- Extremely fast integer sorting compared to comparison-based sorts like `std::sort`.
- Adjustable digit width `w` as a template parameter.
- Uses bucket-based redistribution.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `radix_sort` | Sorts an array `a` of size `n` in place | $O(N \cdot \frac{W}{w})$ |

## Notes
- `W` is the bit size of the type `T` (e.g., 32 for `int`).
- `w` is the bucket width (number of bits per pass, defaults to 4).
- The width of the type must be divisible by $2w$.
