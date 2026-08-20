# Configuration Precedence Checklist

When debugging configuration behavior, evaluate sources in a fixed order and document the winning value in diagnostics.

## Checklist

1. Identify the command and target scope.
2. Load the supported configuration sources.
3. Apply the documented precedence rules.
4. Normalize paths, sizes, durations, and category names.
5. Report the effective configuration when verbose diagnostics are requested.
