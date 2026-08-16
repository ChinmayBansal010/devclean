# Performance guidance

Scanning a large cache can be dominated by filesystem metadata operations rather than application CPU time.

## Practical guidance

- Use name filters when only a small set of caches is relevant.
- Use `--min-size` and `--max-size` to reduce downstream analysis and cleanup work.
- Prefer JSON output for automation rather than repeatedly parsing human tables.
- Benchmark on the same filesystem when comparing builds.
- Treat network-mounted paths as a separate performance class.

The scanner should prioritize correctness and bounded filesystem traversal over aggressive concurrency. Parallel traversal must not be introduced without validating interruption, error propagation, and thread safety of platform backends.
