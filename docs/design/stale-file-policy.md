# Stale File Policy

Stale cleanup operates at file granularity where the platform implementation supports reliable modification-time inspection.

## Age calculation

The cutoff is calculated once at the start of a cleanup operation. Every candidate is compared against that fixed cutoff, preventing files from moving between stale and fresh categories during a long scan.

## Boundary condition

A file exactly at the cutoff is considered stale. This makes the rule deterministic and avoids ambiguity caused by repeated timestamp comparisons.

## Unreadable metadata

If modification time cannot be obtained, the file is excluded from stale cleanup. A missing timestamp is not evidence that a file is safe to delete.

## Directories

Directory metadata alone should not cause recursive deletion in stale mode. Individual files should be evaluated where possible, while empty directories may be removed only as a cleanup consequence if the filesystem layer explicitly permits it.

## Reporting

Reports should distinguish scanned files, stale candidates, skipped files, and successfully removed files.
