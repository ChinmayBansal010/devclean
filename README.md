# devclean

`devclean` is a cross-platform C++23 CLI for scanning, analyzing, and cleaning developer cache directories without hard-coding tool logic into commands. The project uses a registry-driven cache catalog, plugin extensions, persistent scan history, and cross-platform packaging hooks.

## Highlights

- Registry-driven cache definitions with plugin and config overrides
- Scan, analyze, stats, clean, doctor, and version commands
- Recommend and dashboard commands for health scoring and cleanup guidance
- Target-based cleanup and stale-file cleanup
- Safe cleanup mode that excludes active, warned, and protected locations
- Interactive cleanup with JSON output for automation
- Safety checks for active Docker, IDE, and package-manager workflows
- Developer-environment diagnostics with `devclean doctor`
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

### Using vcpkg

This repository includes a local `vcpkg` checkout and a root `vcpkg.json` manifest. To build with vcpkg:

```bash
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

When you build with the vcpkg toolchain, `nlohmann-json` is installed automatically from the manifest.

### Platform notes

- Linux: requires `HOME` and scans cache roots under `~/.cache` by default.
- macOS: uses `~/Library/Caches` for common app and tool caches.
- Windows: uses `%USERPROFILE%`, `%LOCALAPPDATA%`, and `%APPDATA%` where applicable.

### Dependencies

- CMake 3.20+
- C++23 compiler
- `nlohmann_json`
- Optional: `vcpkg` for reproducible dependency management

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
devclean recommend
devclean dashboard
devclean clean --dry-run --exclude npm
devclean clean --safe --target 10GB
devclean clean --safe --stale 30d
devclean doctor
devclean doctor --json
devclean stats --json
devclean version
```

### Cleanup controls

`--target` limits a cleanup operation to the requested amount of cache data. For example:

```bash
devclean clean --safe --target 10GB
```

`--stale` switches cleanup to file-level removal and only removes files older than the requested duration:

```bash
devclean clean --safe --stale 30d
```

Supported duration units are seconds (`s`), minutes (`m`), hours (`h`), days (`d`), and weeks (`w`).

`--safe` excludes caches marked active, caches with active-tool warnings, and protected filesystem locations.

### Developer diagnostics

`devclean doctor` checks common developer tools and reports whether they are installed and currently running. Use `--json` for automation and scripts.

### Cache health and trends

`devclean recommend` prints a health score, growth trend, and cleanup recommendations ranked by impact and safety.

`devclean dashboard` opens an interactive terminal dashboard. Press Enter to refresh the scan and `q` to quit.

### Example output

`scan` shows the cache name, size, file count, category, warning count, and resolved path. This helps you confirm whether the tool is using the expected cache root on your platform.

For the full command and option reference, see [CLI Reference](docs/cli.md).

## Safety

`devclean` refuses destructive cleanup for protected locations such as:

- `/`
- home directories
- Windows system roots
- `Program Files`

Safe cleanup adds another layer by excluding active caches and locations associated with running tools or safety warnings.

Plugins and config-defined cache paths must be absolute and must not point at protected directories.

If an environment variable points `devclean` at a custom cache location, that path is preferred during scan and clean operations.

## Configuration

Configuration is loaded automatically from:

- Linux/macOS: `~/.config/devclean/config.json`
- Windows: `%APPDATA%/devclean/config.json`

See [`docs/config.example.json`](docs/config.example.json) for a complete custom-cache example.

Useful fields:

- `disabledCaches`: turn off selected cache definitions.
- `ignoredCaches`: keep a cache out of normal listings and cleanup.
- `defaultSort`: change the default scan sort order.
- `defaultCategory`: pre-filter scans by category.
- `customCaches`: add your own cache definitions with absolute paths.

## Project docs

- [CLI Reference](docs/cli.md)
- [Configuration example](docs/config.example.json)
- [Architecture](docs/architecture.md)
- [Performance Notes](docs/performance.md)
- [Plugin guide](docs/plugins.md)
- [Testing guide](docs/testing.md)
- [Troubleshooting](docs/troubleshooting.md)
- [Exit codes](docs/exit-codes.md)
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
