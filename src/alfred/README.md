# Alfred XCPC Template Library

This directory is the library body. The code is split by problem domain and
uses umbrella headers for convenient submission and printing.

## Layout

- `core/`: types, random, IO, and shared helpers.
- `algorithm/`: general algorithms that do not fit another domain.
- `data_structure/`: Fenwick trees, DSU, sparse tables, and dynamic trees.
- `graph/`: SCC, EBCC, max flow, min-cost flow, and matching.
- `math/`: number theory, combinatorics, linear algebra, and transforms.
- `math/poly.hpp`: polynomial and formal power series operations.
- `string/`: hashing, KMP, Z, suffix array, automata, and Lyndon tools.
- `geometry/`: 2D and 3D geometry.

## Umbrella headers

- `all.hpp`: every stable library module.

Each domain also has its own `all.hpp`.
