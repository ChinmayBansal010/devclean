# Scan Consistency

A scan represents one logical observation of the developer environment.

## Requirements

- cache identities must remain stable between scans
- each result records the observation time
- filesystem failures are represented explicitly rather than silently converted to zero
- repeated scans of unchanged input should produce equivalent measurements within documented filesystem limits
- history updates occur only after the scan completes successfully

Partial failures must not corrupt previous history. A failed or interrupted scan may be reported, but it must not overwrite a known-good snapshot with incomplete data.
