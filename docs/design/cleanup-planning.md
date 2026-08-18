# Cleanup Planning Contract

Cleanup planning is intentionally separate from cleanup execution.

## Planning stages

1. Normalize command-line limits.
2. Collect eligible cache candidates.
3. Remove candidates blocked by safety policy.
4. Rank the remaining candidates deterministically.
5. Stop selection when the requested recovery target is satisfied.
6. Emit a plan that can be reviewed or serialized.

## Dry-run behavior

Dry-run mode must execute every planning step but perform no filesystem mutation. The displayed recovery estimate should be calculated from the same plan used by real cleanup.

## Partial recovery

A target is a ceiling, not a promise that every byte will be recovered. Filesystem permissions, concurrent changes, and cache-specific restrictions can reduce actual recovery. The final cleanup result should therefore report requested, planned, and realized bytes separately.

## Stability

The planner must not depend on unordered container iteration for user-visible ordering. Stable ordering is required for reproducible reports and tests.
