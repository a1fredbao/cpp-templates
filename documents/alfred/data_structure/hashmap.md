---
title: HashMap
documentation_of: ./src/alfred/data_structure/hashmap.hpp
---

## Description

A fast custom hash map implementation for integer keys, optimized for competitive programming to avoid hash collisions. It uses Splitmix hash internally for key distribution.

## Usage Example

```cpp
#include "hashmap.hpp"
#include <iostream>

int main() {
    HashMap<long long, int> mp;
    mp.init(10000); // Initialize hash map with expected size
    mp.set(123456789, 42);
    mp.inc(123456789, 10);
    std::cout << mp.get(123456789) << "\n"; // Outputs 52
    mp[987654321] = 100;
    std::cout << mp[987654321] << "\n"; // Outputs 100
    return 0;
}
```

## Main Features
- Collision-resistant integer hashing using `Splitmix`.
- Fast operations using open-chaining and power-of-two mask array resizing.
- Simple, map-like interface.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `init` | Initializes the hash map with a specified size capacity | $O(S)$ where $S$ is the next power of two |
| `set` | Sets the value for a key | $O(1)$ average |
| `inc` | Increments the value of a key | $O(1)$ average |
| `get` | Retrieves value for a key (does not create elements) | $O(1)$ average |
| `operator[]` | Accesses/Creates the element for a key | $O(1)$ average |

## Notes
- `init(sz)` must be called before using the map.
- Ideal for situations where `std::unordered_map` is too slow or vulnerable to anti-hash tests.
