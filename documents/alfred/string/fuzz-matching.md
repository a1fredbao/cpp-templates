---
title: Fuzzy String Matching
documentation_of: ./src/alfred/string/fuzz-matching.hpp
---

## Description

Implements fuzzy string matching with a wildcard character (e.g. `*` or `?` representing any character). It leverages FFT/polynomial convolution to compute mismatched indices efficiently.

## Usage Example

```cpp
#include "fuzz-matching.hpp"
#include <iostream>
#include <vector>

int main() {
    std::string text = "abacaba";
    std::string pattern = "a?a"; // '?' is the wildcard character
    auto matches = fuzz_matching(pattern, text, '?');
    for (auto idx : matches) {
        std::cout << "Match found at index: " << idx << "\n";
    }
    return 0;
}
```

## Main Features
- Matches a pattern with a wildcard against a text.
- Formulates the wildcard matching problem as polynomial multiplications (using 3-prime NTT convolution under the hood).
- Returns 0-based indices of all matching positions in the text.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `fuzz_matching` | Finds all matches of `pat` in `txt` using `fuzz` as the wildcard character | $O((N + M) \log (N + M))$ |

## Notes
- `fuzz` represents the wildcard character.
- Under the hood, characters are mapped to integers, and three FFT convolutions are executed to compute mismatch distance at each position.
