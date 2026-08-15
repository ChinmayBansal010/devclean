# Filtering

Use the same size range and active-only filters across commands that support cache selection.

```text
devclean scan --active-only --min-size 250MB --max-size 2GB
devclean stats --min-size 1GB --max-size 4GB
devclean analyze --min-size 250MB --max-size 2GB
devclean clean --dry-run --min-size 500MB --max-size 2GB
```

A dry run is recommended before destructive cleanup.
