# Testing Strategy

Testing is organized around contracts rather than implementation details.

## Unit tests

Pure policy, formatting, parsing, scoring, and planning logic should be tested without touching the real user filesystem.

## Integration tests

CLI tests exercise command dispatch, argument combinations, output modes, and exit behavior.

## Filesystem tests

Filesystem behavior should use temporary fixtures and verify protected paths, stale entries, inaccessible entries, symbolic links, and nested directories.

## Regression tests

Every discovered safety or portability defect should become a focused regression case. Windows path normalization and platform-specific environment handling deserve dedicated coverage.

## Release verification

A release candidate should run the complete test suite, sanitizer builds where supported, static analysis, packaging, and representative CLI smoke tests.
