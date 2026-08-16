# Scan result contract

A scan result represents one cache definition and its observed filesystem state.

## Stable fields

- `name`: canonical cache name
- `aliases`: accepted aliases
- `location`: resolved filesystem path
- `category`: cache category
- `enabled`: whether the cache definition is enabled
- `active`: whether a related tool is detected
- `found`: whether the cache directory exists
- `bytes`: observed size in bytes
- `files`: file count
- `directories`: directory count
- `modified`: last modification timestamp when available
- `age`: elapsed age derived from the modification time
- `growthBytes`: size delta relative to the previous stored snapshot
- `warnings`: tool-specific warnings
- `skipped`: whether configuration or inspection prevented normal processing
- `error`: diagnostic text when processing fails

Consumers should treat missing directories and skipped entries as valid scan outcomes rather than process failures.
