# Alfred XCPC Template Library

This directory is the library body. The code is split by problem domain and
uses umbrella headers for convenient submission and printing.

## Layout

- `core/`: types, constants, random, IO, bit utilities, and shared helpers.
- `algorithm/`: general algorithms that do not fit another domain.
- `data_structure/`: Fenwick trees, segment trees, DSU, persistent structures,
  balanced trees, and dynamic trees.
- `graph/`: SCC, EBCC, max flow, min-cost flow, matching, shortest paths.
- `math/`: number theory, combinatorics, linear algebra, and transforms.
- `math/polynomial/`: NTT and formal power series operations.
- `string/`: hashing, KMP, Z, suffix array, automata, and Lyndon tools.
- `tree/`: LCA, HLD, tree difference, DSU on tree, centroid decomposition.
- `geometry/`: 2D and 3D geometry.
- `config/`: compile-time and IO configuration.
- `online/`: online-contest-only harnesses.
- `onsite/`: compact onsite-contest bundles.

## Umbrella headers

- `all.hpp`: every stable library module.
- `all_debug.hpp`: `all.hpp` plus the debug printer. Online only.
- `online.hpp`: `all.hpp` plus online-only harnesses.
- `onsite.hpp`: compact subset intended for paper printing.

Each domain also has its own `all.hpp`.
