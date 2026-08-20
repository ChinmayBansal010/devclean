# Report Determinism

Equivalent scans should produce equivalent machine-readable ordering and aggregate values when the filesystem state is unchanged.

Use explicit sorting and stable identifiers rather than relying on filesystem traversal order. Determinism makes regression tests, history comparisons, and automation more reliable.
