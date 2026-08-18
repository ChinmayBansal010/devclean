# Reliability Guidelines

## Filesystem errors

A scan should continue when an individual cache entry cannot be read, while preserving the failure as a warning in the result. A single inaccessible path should not invalidate unrelated cache results.

## Cleanup failures

Cleanup operations should report each failed item independently and retain enough information to explain why it was skipped or rejected.

## Determinism

Given the same scan input, policy, and filesystem state, candidate ordering should be stable. Deterministic output makes reports, tests, and troubleshooting substantially easier.

## Partial completion

A cleanup is allowed to partially succeed. The result should distinguish planned, completed, skipped, and failed operations instead of collapsing everything into a single success flag.
