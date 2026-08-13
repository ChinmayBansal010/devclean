# CLI Reference

## Command shape

```text
devclean <command> [targets...] [options]
```

Targets can be cache names or aliases. When no target is supplied, the command uses the configured cache catalog.

## Version and help

```bash
devclean version
devclean --version
devclean -V
devclean --help
devclean -h
```

`--version` and `-V` print the installed version without requiring a command. `--help` and `-h` display the command reference.

## Scan

```bash
devclean scan
devclean scan python cargo
devclean scan --category python
devclean scan --active-only
devclean scan --min-size 250MB
devclean scan --max-size 2GB
devclean scan --min-size 250MB --max-size 2GB
devclean scan --sort size --reverse
devclean scan --exclude npm --exclude yarn
devclean scan --json
```

`scan` resolves configured cache locations, reports discovered size and file counts, records a scan snapshot, and applies category, activity, size, exclusion, and sort filters to the displayed results.

`--min-size` and `--max-size` accept whole-byte values and binary units such as `512KB`, `250MB`, `2GB`, and `1TB`. When both are supplied, only found caches inside the inclusive size range are included.

## Analyze

```bash
devclean analyze
devclean analyze --active-only
devclean analyze --min-size 250MB --max-size 2GB --report json
devclean analyze --report markdown
```

Use `--report` when the output needs to be consumed by another tool or saved as documentation. Analysis respects the same activity and size-range filters as scanning.

## Statistics

```bash
devclean stats
devclean stats --active-only --min-size 1GB --max-size 4GB
devclean stats --json
```

`stats` summarizes total disk usage, files, directories, largest caches, and category totals for the selected cache set.

## Cleanup

```bash
devclean clean --dry-run
devclean clean --dry-run python
devclean clean --dry-run --min-size 500MB --max-size 2GB --sort size --reverse
devclean clean --force --active-only --exclude npm
```

Use `--dry-run` before destructive cleanup. `--force` bypasses interactive confirmation where the command permits it. Protected locations remain blocked by the safety layer.

## Common options

| Option | Purpose |
| --- | --- |
| `--category <name>` | Filter by cache category |
| `--exclude <name>` | Exclude a cache from the operation |
| `--active-only` | Include only caches associated with installed/active tools |
| `--min-size <size>` | Include only found caches at or above a size threshold |
| `--max-size <size>` | Include only found caches at or below a size threshold |
| `--sort <name>` | Sort output by name, size, or modification time |
| `--reverse` | Reverse the selected sort order |
| `--json` | Emit machine-readable JSON |
| `--verbose` | Show additional operation progress |
| `--dry-run` | Preview cleanup without deleting data |
| `--force` | Skip interactive cleanup confirmation |
| `--help`, `-h` | Show command help |
| `--version`, `-V` | Print the current version |

## Configuration

The configuration file is loaded from `~/.config/devclean/config.json` on Linux/macOS and `%APPDATA%/devclean/config.json` on Windows. See the main README for the supported schema and custom cache configuration.
