# Alfred XCPC Templates

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/a1fredbao/cpp-templates)

A contest-oriented C++ template library. The library body, verification
suite, and PDF tooling are intentionally separated.

## Repository layout

```text
src/alfred/     C++ library body
verify/         Library Checker, AOJ, and standalone verification
tools/pdf/      PDF generator, manifests, and LaTeX templates
tools/install/  Installation and uninstallation scripts
thirdparty/     Jiangly and Watashi reference material
```

## Library

The library is organized by problem domain:

```text
src/alfred/core/             types, random, IO helpers
src/alfred/algorithm/        general algorithms
src/alfred/data_structure/   static, dynamic, persistent structures
src/alfred/graph/            connectivity, flows, matchings
src/alfred/math/             number theory, algebra, transforms
src/alfred/math/poly.hpp      polynomial and formal power series
src/alfred/string/           hashing and string structures
src/alfred/geometry/         point, convexity, and intersection algorithms
```

Umbrella headers:

- `src/alfred/all.hpp`: all stable modules.

## Verification

Verification lives under `verify/` and mirrors the source of truth:

- `verify/library_checker/` for Library Checker problems.
- `verify/aizu/` for AOJ problems.
- `verify/standalone/` for randomized and brute-force tests.

## PDF generation

The canonical generator lives under `tools/pdf/`:

```sh
pip install -r tools/pdf/requirements.txt
python -m tools.pdf --preset both --output-dir build
```

The root `main.py` is a small compatibility entry point:

```sh
python main.py --preset both --output-dir build
```

See `tools/pdf/README.md` for custom config and manifest regeneration.

## Installation and uninstallation

The installation script is located under `tools/install/`:

```sh
python tools/install/install.py --install                   # Installs to ~/.xcpc by default
python tools/install/install.py --install --path /usr/local # Installs to /usr/local

python tools/install/install.py --uninstall                 # Uninstalls from ~/.xcpc by default
python tools/install/install.py --uninstall --path /usr/local # Uninstalls from /usr/local
```

## Third-party references

`thirdparty/` contains archived Jiangly and Watashi reference material. It is
not part of the library build and must not be included by `src/alfred`.

## Development rules

- Prefer small interfaces with deep implementations.
- Keep core headers self-contained and free of mutable global state.
- Return results instead of printing from library modules.
- Add one verification problem for every reusable algorithm or structure.
