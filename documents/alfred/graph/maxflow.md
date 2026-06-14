---
title: Max Flow (Dinic)
documentation_of: ./src/alfred/graph/maxflow.hpp
---

## Description

Dinic's algorithm for finding the maximum flow in a flow network. It supports bipartite matching, generic network flow, and edge reconstruction.

## Usage Example

```cpp
#include "maxflow.hpp"
#include <iostream>

int main() {
    MaxFlow<int> mf(4);
    mf.add(0, 1, 3);
    mf.add(0, 2, 2);
    mf.add(1, 2, 1);
    mf.add(1, 3, 2);
    mf.add(2, 3, 3);
    std::cout << "Max Flow: " << mf.maxflow(0, 3) << "\n"; // Outputs 4
    return 0;
}
```

## Main Features
- Dynamic generic capacities (`T`).
- Standard Dinic algorithm using BFS levels and DFS blocking flow.
- Support for retrieving final flows and residual capacities of edges.

## Important Functions and Time Complexity

| Function | Description | Time Complexity |
| --- | --- | --- |
| `add` | Adds a directed edge with capacity `c` | $O(1)$ |
| `maxflow` | Calculates the maximum flow from source `s` to sink `t` | $O(V^2 E)$ generally |
| `edges` | Reconstructs the edges and their flows | $O(E)$ |

## Notes
- Extremely fast in practice, typically much faster than the worst-case bound.
- For bipartite matching, the time complexity is $O(E \sqrt{V})$.
