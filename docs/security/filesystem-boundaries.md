# Filesystem safety boundaries

Cleanup is destructive and must remain scoped to configured cache locations.

## Rules

1. Never recursively delete a path that was not resolved from a cache definition or explicitly configured custom cache.
2. Treat missing paths as successful no-op cleanup candidates.
3. Reject filesystem roots and protected locations before deletion.
4. Keep `--dry-run` side-effect free.
5. Surface permission failures instead of silently reporting successful deletion.
6. Preserve the resolved path in machine-readable output so callers can audit what would be changed.

A cache being large is not sufficient reason to delete it. The cache must also be selected by the user's command and pass the safety checks.
