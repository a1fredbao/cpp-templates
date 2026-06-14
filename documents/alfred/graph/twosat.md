---
title: 2-SAT
documentation_of: ./src/alfred/graph/twosat.hpp
---

## Description

Resolves the 2-Satisfiability problem using Tarjan's strongly connected components algorithm.

## Usage Example

```cpp
#include "twosat.hpp"
#include <iostream>

int main() {
    TwoSAT solver(2); // 2 variables: 0 and 1
    solver.add(0, true, 1, false); // (x0 is true) OR (x1 is false)
    solver.set(0, false); // x0 must be false
    solver.init();
    if (solver.has_solution()) {
        std::cout << "Has solution!\n";
        auto sol = solver.solve();
        std::cout << "x0 = " << sol[0] << ", x1 = " << sol[1] << "\n";
    } else {
        std::cout << "No solution!\n";
    }
    return 0;
}
```

## Main Features
- Tarjan-based SCC search for 2-SAT implications.
- High-level constraints helper functions like `add`, `conduct`, `same`, `diff`, and `set`.
- Generates a valid assignment if a solution exists.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `conduct` | Add implication constraint: $x = a \Rightarrow y = b$ | $O(1)$ |
| `add` | Add clause constraint: $x = a \lor y = b$ | $O(1)$ |
| `same` | Add constraint: $x = y$ | $O(1)$ |
| `diff` | Add constraint: $x \neq y$ | $O(1)$ |
| `set` | Force variable assignment: $x = v$ | $O(1)$ |
| `init` | Computes SCCs on the implication graph | $O(V + E)$ |
| `has_solution` | Checks if a valid assignment is possible | $O(V)$ |
| `solve` | Generates a valid boolean assignment for variables | $O(V)$ |

## Notes
- Variables are 0-indexed.
- The implication graph size is $2N$.
