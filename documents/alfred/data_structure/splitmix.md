---
title: Splitmix Hash
documentation_of: ./src/alfred/data_structure/splitmix.hpp
---

## Description

A very fast 64-bit and 32-bit hash function (Splitmix64/Splitmix32), useful for preventing hash collision hacking in hash maps or for random number generators.

## Usage Example

```cpp
#include "splitmix.hpp"
#include <iostream>

int main() {
    Splitmix<unsigned long long> hasher;
    unsigned long long h = hasher(12345);
    std::cout << "Hash: " << h << "\n";
    return 0;
}
```

## Main Features
- Provides fast, high-quality pseudo-random hashing.
- Optimized specialization for `unsigned long long` and `unsigned int` to support older C++ standards without `if constexpr`.
- Effective for safeguarding custom hash maps against collision attacks.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `operator()` | Hashes the given key value | $O(1)$ |

## Notes
- Safe to use as the hash function for custom hash tables (e.g. `HashMap`).
