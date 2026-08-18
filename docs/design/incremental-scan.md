# Incremental Scan Strategy

Incremental scanning should reduce repeated work while preserving correctness.

## Cache identity

A cache is identified by its stable registry name and normalized root. Metadata from a previous scan may be reused only when the root remains equivalent.

## Reuse conditions

Directory metadata, modification times, and configured scan policy can be used to determine whether a subtree is eligible for reuse. If any required signal is unavailable, the scanner should fall back to a full traversal rather than trust stale state.

## Invalidation

Configuration changes, plugin changes, filesystem errors, and explicit user requests for a fresh scan invalidate cached scan data.

## Safety

Incremental state is an optimization, never an authority for cleanup eligibility. Before mutation, the current filesystem must still be validated.
