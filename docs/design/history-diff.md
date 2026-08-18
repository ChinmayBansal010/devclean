# History Diff Model

History comparisons should answer what changed between two scans without requiring callers to understand the persistence format.

## Matching

Caches are matched by their stable registry name rather than display labels or filesystem path strings. This allows a configured path to change without turning one cache into two unrelated historical records.

## Changes

A diff can report added caches, removed caches, size increases, size decreases, activity changes, and metadata changes. Byte deltas should be signed integers so growth and shrinkage are represented symmetrically.

## Missing snapshots

A missing historical entry is different from an entry whose size is zero. The distinction matters when a cache is newly discovered or temporarily unavailable.

## Determinism

Diff output should use stable cache ordering. Human-readable output can sort by magnitude, while machine-readable output should preserve the documented deterministic order.
