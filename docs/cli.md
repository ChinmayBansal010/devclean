# CLI Reference

## Command shape

```text
devclean <command> [targets...] [options]
```

Targets can be cache names or aliases. When no target is supplied, the command uses the configured cache catalog.

## Scan

```bash
devclean scan
devclean scan python cargo
devclean scan --category python
devclean scan --sort size --reverse
devclean scan --exclude npm --exclude yarn
devclean scan --json
```

`scan` resolves configured cache locations, reports discovered size and file counts, records a scan snapshot, and applies category/exclusion/sort filters to the displayed results.

## Analyze

```bash
devclean analyze
devclean analyze --report json
devclean analyze --report markdown
```

Use `--report` when the output needs to be consumed by another tool or saved as documentation.

## Statistics

```bash
devclean stats
devclean stats --json
```

`stats` summarizes total disk usage, files, directories, largest caches, and category totals.

## Cleanup

```bash
devclean clean --dry-run
devclean clean --dry-run python
devclean clean --force --exclude npm
```

Use `--dry-run` before destructive cleanup. `--force` bypasses interactive confirmation where the command permits it. Protected locations remain blocked by the safety layer.

## Common options

| Option | Purpose |
| --- | --- |
| `--category <name>` | Filter by cache category |
| `--exclude <name>` | Exclude a cache from the operation |
| `--sort <name>` | Sort scan output by name, size, or modification time |
| `--reverse` | Reverse the selected sort order |
| `--json` | Emit machine-readable JSON |
| `--verbose` | Show additional operation progress |
| `--dry-run` | Preview cleanup without deleting data |
| `--force` | Skip interactive cleanup confirmation |
| `--help`, `-h` | Show command help |

## Configuration

The configuration file is loaded from `~/.config/devclean/config.json` on Linux/macOS and `%APPDATA%/devclean/config.json` on Windows. See the main README for the supported schema and custom cache configuration.
