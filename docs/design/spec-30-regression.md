# Regression Strategy

Regression tests should cover parser behavior, policy decisions, scanner normalization, cleanup planning, and output stability.

A behavioral change requires a focused regression case before release. Platform-specific cases should remain isolated so unsupported environments do not become false failures.