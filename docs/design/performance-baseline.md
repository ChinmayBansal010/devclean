# Performance Baseline

Performance measurements should use realistic developer cache trees rather than empty temporary directories.

## Metrics

The primary scan metrics are wall-clock duration, files visited, directories visited, bytes measured, and cache count. Cleanup adds planned candidates, mutation duration, and realized bytes.

## Comparability

Benchmarks should record operating system, filesystem type, build configuration, compiler version, and whether filesystem metadata is warm in the operating-system cache.

## Regression policy

A performance regression should be investigated when a representative workload becomes materially slower without a corresponding correctness or safety benefit. Measurements should be repeated before drawing conclusions because filesystem workloads have high variance.

## Optimization order

Correctness and safety take precedence over micro-optimizations. Prefer reducing redundant traversal and unnecessary metadata calls before adding complicated caching or concurrency.
