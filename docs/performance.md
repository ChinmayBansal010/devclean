# Performance Notes

`devclean` is designed to keep cache discovery predictable while avoiding unnecessary work during normal CLI use.

## Scanning

- Cache definitions are resolved through the registry before filesystem traversal.
- Results are collected once and reused by command-level filtering and sorting.
- JSON output is generated from the same scan results as human-readable output.
- Scan history stores bounded snapshots so repeated scans do not grow without limit.

## Cleanup

Cleanup should normally start with `--dry-run`. The safety layer rejects protected locations before recursive deletion is attempted.

## Large cache trees

Filesystem traversal cost is primarily proportional to the number of files and directories encountered. For large caches, targeting a specific ecosystem or using exclusions can reduce unnecessary traversal.

## Diagnostics

Use `--verbose` for additional command progress and `stats --json` when integrating measurements into scripts or dashboards.

Performance-sensitive changes should be validated with the repository benchmark target and the normal CTest suite before release.
