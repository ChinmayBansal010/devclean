# CLI Exit Codes

Exit codes are part of the command-line interface contract because shell scripts and CI systems consume them.

## Principles

- Zero means the requested operation completed successfully.
- Invalid arguments are reported separately from runtime failures.
- A successful dry run remains successful even when it proposes no cleanup.
- Partial cleanup should be distinguishable from a complete cleanup when machine-readable consumers need that distinction.
- Diagnostics findings do not automatically imply command failure.

## Stability

Existing meanings should not be reused for unrelated conditions. New conditions should receive documented codes or be represented in structured output while retaining a compatible general status.

## Scripts

Documentation and examples should encourage callers to inspect the exit code rather than parse human-readable output. JSON should expose the same high-level status explicitly.
