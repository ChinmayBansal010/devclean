# Plugin Guide

## Location

Plugin files are loaded from:

- Linux/macOS: `~/.config/devclean/plugins`
- Windows: `%APPDATA%/devclean/plugins`

Each plugin is a single JSON file describing one cache entry.

## Supported fields

```json
{
  "name": "tool-cache",
  "category": "build",
  "description": "Custom build cache",
  "enabled": true,
  "priority": 5,
  "aliases": ["tool"],
  "environmentVariables": ["TOOL_CACHE_DIR"],
  "osSupport": ["linux", "windows"],
  "path": "/absolute/path/to/cache",
  "cachePaths": ["/absolute/path/to/cache-alt"]
}
```

## Validation

Plugins are rejected when:

- `name` contains unsafe characters
- the cache path is relative
- the cache path contains `..`
- the cache path points to a protected directory
- the platform-specific primary path is missing

## Best practices

- Keep plugin names stable and lowercase
- Prefer one plugin file per tool cache
- Use absolute cache paths only
- Keep aliases short and unique
- Treat `cachePaths` as supplementary lookup paths, not overrides for protected paths
