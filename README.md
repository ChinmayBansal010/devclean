# devclean

A cross-platform C++23 developer cache cleaner.

## Features

- Scan common developer cache directories
- Analyze large and stale caches with recommendations
- Clean selected caches safely with warnings for active Docker, IDE, and package-manager workflows
- Support category-based filtering with `--category`
- Support exclusion with `--exclude`
- Support sorting by `name`, `size`, or `modified` and `--reverse`
- Support partial search terms and alias matching
- Produce scan summaries, JSON output, and markdown/html/csv/json reports
- Load and migrate configuration from the user config directory
- Build packaging and release automation hooks for Linux and Windows distributions

## Usage

```bash
devclean scan
devclean scan --category python
devclean scan pip
devclean clean --dry-run --exclude pip
devclean stats
devclean version
```

## Configuration

Configuration is loaded automatically from:
- Linux: `~/.config/devclean/config.json`
- Windows: `%APPDATA%/devclean/config.json`

Supported keys include:
- `disabledCaches = ["gradle", "cargo"]`
- `ignoredCaches = ["gradle", "cargo"]`
- `defaultSort = "size"`
- `defaultCategory = "python"`
- `schemaVersion = 3`
