# Large Tree Handling

Large caches should avoid retaining every file path in memory when only aggregate size and age are required.

Traversal may stream measurements into bounded accumulators. File-level stale cleanup requires a separate candidate phase. Both paths must preserve overflow checks and deterministic aggregate results.