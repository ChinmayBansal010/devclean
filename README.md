# devclean

`devclean` is a cross-platform C++23 CLI for scanning, analyzing, and cleaning developer cache directories without hard-coding tool logic into commands. The project uses a registry-driven cache catalog, plugin extensions, persistent scan history, and cross-platform packaging hooks.

## Highlights

- Registry-driven cache definitions with plugin overrides
- Scan, analyze, stats, clean, and version commands
- Interactive cleanup with JSON output for automation
- Safety checks for active Docker, IDE, and package-manager workflows
- Report export in Markdown, HTML, CSV, and JSON
- Release packaging for Linux, Windows, and macOS workflows

## Installation

### Build from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
./build/devclean scan
```

### Dependencies

- CMake 3.20+
- C++23 compiler
- `nlohmann_json`

The build falls back to `FetchContent` when `nlohmann_json` is not preinstalled.

## Usage

```bash
devclean scan
devclean scan --category python
devclean analyze --report json
devclean clean --dry-run --exclude npm
devclean stats --json
devclean version
```

## Safety

`devclean` refuses destructive cleanup for protected locations such as:

- `/`
- home directories
- Windows system roots
- `Program Files`

Plugins and config-defined cache paths must be absolute and must not point at protected directories.

## Configuration

Configuration is loaded automatically from:

- Linux/macOS: `~/.config/devclean/config.json`
- Windows: `%APPDATA%/devclean/config.json`

Example:

```json
{
  "schemaVersion": 3,
  "disabledCaches": ["cargo"],
  "ignoredCaches": ["npm"],
  "defaultSort": "size",
  "defaultCategory": "python"
}
```

## Project docs

- [Architecture](docs/architecture.md)
- [Plugin guide](docs/plugins.md)
- [Roadmap](docs/roadmap.md)
- [Contributing](CONTRIBUTING.md)
- [Security policy](SECURITY.md)

## Development

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Optional quality targets:

```bash
cmake --build build --target tidy
cmake --build build --target cppcheck
cmake --build build --target format
```
