# Verification

Every library module is verified through an external judge problem or a
standalone randomized test. Tests are grouped by the source of truth, not by
library directory.

## Layout

- `library_checker/`: Library Checker problems.
- `aizu/`: AOJ problems.
- `standalone/`: randomized and brute-force tests with no external judge.

Each test file must start with a `competitive-verifier: PROBLEM` comment or
clearly state why no external problem exists.

The problem URL must remain on the first line. The repository `.clang-format`
sets `ReflowComments: false` so clang-format does not split URL comments.
