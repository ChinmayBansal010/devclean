# Functional Cache Filtering

The scan command can narrow results using runtime state and cache size filters.

Examples:

```bash
devclean scan --active-only
devclean clean --active-only
devclean scan --min-size 100MB
devclean clean --min-size 1GB
```

These filters are applied after filesystem inspection, so reported sizes and active-tool state reflect the current machine.
