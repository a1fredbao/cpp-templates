---
title: Convolution (NTT/FWT)
documentation_of: ./src/alfred/math/conv.hpp
---

## Description

This file provides polynomial convolution operations. It includes Fast Walsh-Hadamard Transform (FWT) for bitwise logical convolutions (AND, OR, XOR), and Number Theoretic Transform (NTT) for standard polynomial multiplication. It also supports NTT-based multiplication for `long long` integers using 3-prime NTT and CRT.

## Usage Example

```cpp
#include "conv.hpp"
#include <iostream>

using mint = ModInt<998244353>;

int main() {
    std::vector<mint> f = {1, 2, 3};
    std::vector<mint> g = {1, 1};
    auto h = add_conv(f, g); // h = {1, 3, 5, 3}
    for (auto x : h) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    return 0;
}
```

## Main Features
- Standard NTT-based convolution (`add_conv`) for ModInt polynomials.
- Arbitrary modulo `long long` convolution (`add_conv_ll`) using three NTT-friendly primes (754974721, 167772161, 469762049) and Garner's algorithm (CRT).
- Fast Walsh-Hadamard Transform (FWT/IFWT) for AND, OR, XOR convolutions (`and_conv`, `or_conv`, `xor_conv`).

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `add_conv` | Polynomial multiplication under modulo | $O(N \log N)$ |
| `add_conv_ll` | Polynomial multiplication with `long long` coefficients | $O(N \log N)$ |
| `and_conv` | Bitwise AND convolution | $O(N \log N)$ |
| `or_conv` | Bitwise OR convolution | $O(N \log N)$ |
| `xor_conv` | Bitwise XOR convolution | $O(N \log N)$ |

## Notes
- Polynomial sizes are padded to the next power of two during transformations.
- Special brute-force optimization is applied when one of the polynomials is small (size < 128) in `add_conv`.
