# Error Recovery Model

Developer cache trees are mutable and imperfect. Cleanup must expect permissions, missing paths, concurrent deletion, and transient filesystem errors.

## Per-item recovery

A failure affecting one file or cache should be recorded and should not erase successful results from unrelated caches.

## Revalidation

Before a destructive operation, the engine should confirm that the path still resolves to the expected safe root. If it changed or disappeared, the item becomes skipped or changed-before-execution.

## Aggregation

The final result should expose successful bytes, failed operations, skipped operations, and warnings independently. This prevents a partial cleanup from being presented as completely successful.

## User action

Errors should be actionable when possible: permission failures can suggest running with appropriate permissions, while path disappearance should generally be treated as already resolved.
