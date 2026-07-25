# Architecture

## Overview

`devclean` is organized around a small set of stable modules:

- `core`: application startup, argument parsing, config loading, command dispatch, scan history
- `scanner`: cache registry, plugin loading, scan orchestration, scan result structures
- `cleaner`: deletion orchestration
- `platform`: filesystem and runtime/tool detection
- `commands`: user-facing command implementations
- `utils`: formatting, terminal helpers, threading utilities

## Flow

1. `main` delegates to `Application`
2. `Application` delegates to `CommandDispatcher`
3. Commands load config and invoke `ScannerEngine` or `CleanEngine`
4. `ScannerEngine` merges built-in registry entries with config and plugin entries
5. `Filesystem` performs traversal and deletion work behind the platform abstraction

## Design rules

- Cache definitions remain registry-driven
- Commands stay thin and mostly orchestrate
- Filesystem mutations go through `platform/Filesystem`
- Plugins can extend the registry but cannot bypass path validation
- Config migration must preserve backward compatibility

## Safety model

- Protected directories are never deleted
- Top-level symlink cleanup is rejected
- Plugin cache paths must be absolute and non-protected
- Custom cache definitions are filtered during config validation

## Performance notes

- Directory traversal collects file counts, size totals, and file-type sizes in one pass
- Scan history stores previous snapshots for lightweight growth reporting
- Registry merging avoids duplicate cache definitions by normalized name and alias
