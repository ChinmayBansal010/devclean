# Release Gates

A release should satisfy explicit engineering gates before a version tag is created.

## Required gates

- Clean configure on supported platforms
- Successful build in the release configuration
- Complete test suite
- No newly introduced compiler warnings treated as errors
- Stable CLI help and version output
- Structured output compatibility reviewed
- Cleanup safety checks exercised in dry-run tests
- Packaging metadata validated

## Documentation

User-facing behavior must be documented before release. Breaking changes require an explicit migration note.

## Reproducibility

Release builds should record the source revision, version, compiler family, and build configuration. Generated artifacts should be produced from a clean checkout when practical.

## Failure policy

A release gate failure blocks tagging. Warnings that are known and accepted should be documented rather than silently ignored.
