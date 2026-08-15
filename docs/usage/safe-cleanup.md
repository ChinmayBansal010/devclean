# Safe cleanup workflow

Inspect candidates first:

```bash
devclean scan --json
devclean clean --dry-run
```

Narrow the operation with category, active-only, and size filters. Only run the non-dry cleanup after reviewing the candidates.
