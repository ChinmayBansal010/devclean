# Report Schema

Reports should expose a common model regardless of whether the selected format is terminal text, JSON, CSV, Markdown, or HTML.

## Core fields

A report can contain command metadata, scan timestamp, platform information, cache entries, totals, warnings, and cleanup results.

Each cache entry should retain its stable name, category, path, byte count, file count, directory count, age information, activity state, safety state, and recommendation when available.

## Numeric representation

Byte counts should remain integers in structured formats. Human-readable renderers may convert them to KiB, MiB, GiB, or other display units.

## Compatibility

Adding a field is preferred to changing the meaning of an existing field. Removing or renaming a field should be treated as a compatibility event and documented in release notes.

## Error reporting

Recoverable per-cache failures belong in warnings attached to the relevant entry. Fatal command errors should remain at the top-level result so consumers can distinguish incomplete scans from successful scans with individual warnings.
