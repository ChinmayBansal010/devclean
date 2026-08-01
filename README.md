# devclean

`devclean` is a cross-platform C++23 CLI for scanning, analyzing, and cleaning developer cache directories without hard-coding tool logic into commands. The project uses a registry-driven cache catalog, plugin extensions, persistent scan history, and cross-platform packaging hooks.

## Highlights

- Registry-driven cache definitions with plugin and config overrides
- Scan, analyze, stats, clean, and version commands
- Interactive cleanup with JSON output for automation
- Safety checks for active Docker, IDE, and package-manager workflows
- Report export in Markdown, HTML, CSV, and JSON
- Native cache-path resolution for common developer tools
- Release packaging for Linux, Windows, and macOS workflows

## Installation

### Build from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
./build/devclean scan
```

### Platform notes

- Linux: requires `HOME` and scans cache roots under `~/.cache` by default.
- macOS: uses `~/Library/Caches` for common app and tool caches.
- Windows: uses `%USERPROFILE%`, `%LOCALAPPDATA%`, and `%APPDATA%` where applicable.

### Dependencies

- CMake 3.20+
- C++23 compiler
- `nlohmann_json`

The build falls back to `FetchContent` when `nlohmann_json` is not preinstalled.

### Optional package installs

- Debian/Ubuntu: `sudo apt install cmake g++`
- Fedora: `sudo dnf install cmake gcc-c++`
- macOS: `brew install cmake`
- Windows: install CMake and a C++23-capable compiler such as MSVC or Clang

## Supported caches

`devclean` discovers caches for the most common toolchains and ecosystems:

- Python: `pip`, `pipenv`, `poetry`, `uv`, `pixi`, `mamba`
- JavaScript: `npm`, `pnpm`, `yarn`, `bun`, `nvm`
- Rust: `cargo`, `rustup`
- Java: `gradle`, `maven`
- C/C++: `ccache`, `conan`, `vcpkg`, `cmake`, `meson`, `bazel`
- Containers: `docker-builder`, `docker-volumes`, `podman`
- Editors and IDEs: `vscode`, `jetbrains`

## Usage

```bash
devclean scan
devclean scan --category python
devclean analyze --report json
devclean clean --dry-run --exclude npm
devclean stats --json
devclean version
```

### Example output

`scan` shows the cache name, size, file count, category, warning count, and resolved path. This helps you confirm whether the tool is using the expected cache root on your platform.

## Safety

`devclean` refuses destructive cleanup for protected locations such as:

- `/`
- home directories
- Windows system roots
- `Program Files`

Plugins and config-defined cache paths must be absolute and must not point at protected directories.

If an environment variable points `devclean` at a custom cache location, that path is preferred during scan and clean operations.

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

Useful fields:

- `disabledCaches`: turn off selected cache definitions.
- `ignoredCaches`: keep a cache out of normal listings and cleanup.
- `defaultSort`: change the default scan sort order.
- `defaultCategory`: pre-filter scans by category.
- `customCaches`: add your own cache definitions with absolute paths.

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
