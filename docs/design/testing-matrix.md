# Testing Matrix

The project should validate behavior at several levels rather than relying on one large end-to-end test suite.

## Unit tests

Exercise byte arithmetic, target selection, age cutoffs, configuration precedence, recommendation ordering, and path safety with deterministic inputs.

## Integration tests

Exercise scanner, registry, history, reporting, and cleanup planning together using temporary filesystem trees.

## Platform tests

Run filesystem and process-detection tests on supported operating systems. Platform-specific expectations should be isolated so common behavior remains portable.

## CLI tests

Validate argument parsing, exit status, human-readable output, JSON output, dry-run behavior, and combinations such as safe cleanup with a target or stale threshold.

## Regression policy

Every discovered correctness or safety bug should gain a focused regression test when practical. Tests should assert behavior and invariants rather than implementation details.
