<div align="center">

# devclean

**A cross-platform developer cache analyzer and cleanup CLI built with C++23.**

Scan what is consuming your disk, understand cache growth, diagnose your development environment, and clean safely without relying on a pile of unrelated shell commands.

[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-555555?style=flat-square)](#platform-support)
[![License](https://img.shields.io/badge/license-MIT-2ea44f?style=flat-square)](LICENSE)

</div>

---

## Why devclean?

Developer machines accumulate caches quietly. Package managers, compilers, build systems, containers, IDEs, and language tooling can leave behind tens of gigabytes that are difficult to inspect manually.

`devclean` provides one consistent interface for **discovering, analyzing, and safely cleaning developer caches** across operating systems.

It is designed around a few principles:

- **Visibility first:** scan and understand before deleting anything.
- **Safety by default:** protected locations and risky cache states are handled explicitly.
- **Automation friendly:** JSON output and deterministic command behavior make scripting practical.
- **Extensible:** cache definitions are registry-driven and can be extended through configuration and plugins.
- **Cross-platform:** platform-specific cache resolution is kept behind the application architecture.

## Highlights

| Capability | What it provides |
|---|---|
| Cache scanning | Discover cache locations, sizes, file counts, warnings, and paths |
| Analysis | Inspect cache composition and produce structured reports |
| Health insights | Growth trends, health scoring, and cleanup recommendations |
| Target cleanup | Clean toward a requested storage budget such as `10GB` |
| Stale cleanup | Remove files older than a requested age such as `30d` |
| Safe mode | Exclude active, warned, and protected locations from cleanup |
| Diagnostics | `devclean doctor` checks common developer tools and their activity |
| Interactive dashboard | Inspect cache state through a terminal dashboard |
| JSON output | Integrate scans, diagnostics, statistics, and reports into scripts |
| Plugin support | Extend the cache catalog without hard-coding every tool into commands |
| History | Persist scan information for trend-oriented analysis |
| Packaging | Build/package workflows for Linux, Windows, and macOS |

## Supported commands

```text
devclean scan       Discover developer caches
devclean analyze    Analyze cache usage and export reports
devclean recommend  Generate cleanup recommendations
devclean dashboard  Open the interactive terminal dashboard
devclean clean      Remove selected cache data
devclean doctor     Diagnose the developer environment
devclean stats      Show cache statistics
devclean version    Show the installed version
```

## Quick start

### Build from source

Requirements:

- CMake 3.20+
- A C++23-capable compiler
- `nlohmann_json`

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```

Run a first scan:

```bash
./build/devclean scan
```

Then inspect the environment:

```bash
./build/devclean doctor
./build/devclean recommend
```

For a destructive operation, start with a dry run:

```bash
./build/devclean clean --dry-run
```

When the proposed cleanup is understood, safety constraints can be applied explicitly:

```bash
./build/devclean clean --safe --target 10GB
```

Or clean only files older than a chosen age:

```bash
./build/devclean clean --safe --stale 30d
```

## Usage

### Scan

```bash
devclean scan
devclean scan --category python
devclean scan --json
devclean scan --active-only
```

A scan reports cache identity, size, file count, directory count, category, warnings, and the resolved filesystem path.

### Analyze

```bash
devclean analyze
devclean analyze --report json
devclean analyze --report html
```

Analysis can be used to understand which caches have the greatest storage impact before cleanup.

### Recommendations and dashboard

```bash
devclean recommend
devclean dashboard
```

Recommendations combine cache health information, growth trends, cleanup impact, and safety considerations. The dashboard provides an interactive terminal view and can refresh its scan state.

### Clean safely

```bash
devclean clean --dry-run
devclean clean --safe
devclean clean --safe --target 10GB
devclean clean --safe --stale 30d
```

`--target` limits the cleanup budget. `--stale` performs file-level cleanup based on age. `--safe` adds restrictions around active caches, warnings, and protected paths.

Supported duration units:

```text
s   seconds
m   minutes
h   hours
d   days
w   weeks
```

### Diagnose

```bash
devclean doctor
devclean doctor --json
```

The doctor command checks common developer tools and reports installation/activity information in a format suitable for both humans and automation.

### Automation

Most information-oriented commands support JSON output where applicable:

```bash
devclean scan --json
devclean doctor --json
devclean stats --json
```

This makes `devclean` suitable for shell scripts, CI diagnostics, local maintenance tooling, and other developer automation.

## Supported caches

The cache catalog currently covers common ecosystems and development tooling:

| Category | Examples |
|---|---|
| Python | `pip`, `pipenv`, `poetry`, `uv`, `pixi`, `mamba` |
| JavaScript | `npm`, `pnpm`, `yarn`, `bun`, `nvm` |
| Rust | `cargo`, `rustup` |
| Java | `gradle`, `maven` |
| C/C++ | `ccache`, `conan`, `vcpkg`, `cmake`, `meson`, `bazel` |
| Containers | Docker builder/volumes, `podman` |
| Editors / IDEs | VS Code, JetBrains tooling |

The registry-based design allows the catalog to grow without coupling every cache implementation to the command layer.

## Platform support

`devclean` contains platform-specific path resolution for:

- **Linux:** cache roots under `~/.cache` and relevant environment-defined locations.
- **macOS:** common application/tool caches under `~/Library/Caches`.
- **Windows:** `%USERPROFILE%`, `%LOCALAPPDATA%`, and `%APPDATA%` where applicable.

Packaging is configured for platform-appropriate archive/package formats.

## Safety model

Cleanup is deliberately more constrained than scanning. `devclean` protects critical filesystem locations and can identify cache locations associated with active development tools.

Protected examples include:

- `/`
- home directories
- Windows system roots
- `Program Files`

Additional safety behavior can exclude caches that are active, warned, or otherwise protected.

Custom cache paths supplied through configuration or plugins must be absolute and are checked against protected locations.

**Recommended workflow:**

```text
SCAN → ANALYZE → REVIEW → DRY RUN → SAFE CLEANUP
```

Do not skip directly to deletion because apparently that is how computers acquire both bugs and angry users.

## Configuration

Configuration is loaded automatically from:

- Linux/macOS: `~/.config/devclean/config.json`
- Windows: `%APPDATA%/devclean/config.json`

See [`docs/config.example.json`](docs/config.example.json) for the complete example.

Supported configuration areas include:

- `disabledCaches` - disable selected cache definitions.
- `ignoredCaches` - hide selected caches from normal listings and cleanup.
- `defaultSort` - configure the default scan sort order.
- `defaultCategory` - configure a default scan category.
- `customCaches` - register custom cache definitions with absolute paths.

## Build with vcpkg

The repository includes a local vcpkg checkout and a root `vcpkg.json` manifest.

```bash
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The manifest provides `nlohmann-json` through the vcpkg toolchain.

## Development

Configure a debug build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
```

Run the complete test suite:

```bash
ctest --test-dir build --output-on-failure
```

Optional static-quality targets are available when the corresponding tools are installed:

```bash
cmake --build build --target tidy
cmake --build build --target cppcheck
cmake --build build --target format
```

Sanitizer builds can be enabled with the project's sanitizer option:

```bash
cmake -S . -B build-sanitize \
  -DDEVCLEAN_ENABLE_SANITIZERS=ON \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure
```

## Project structure

```text
include/       Public C++ interfaces
src/core/      Application, configuration, planning, and command infrastructure
src/commands/  CLI command implementations
src/engine/    Cache analysis and supporting engines
src/scanner/   Cache discovery and registry/plugin handling
src/cleaner/   Cleanup execution
src/platform/  Operating-system-specific filesystem/tool behavior
src/utils/     Formatting, strings, terminal, and concurrency utilities
tests/         CLI, unit, and focused tests
cmake/         Compiler warning and sanitizer configuration
docs/          Detailed project documentation
```

## Documentation

- [CLI Reference](docs/cli.md)
- [Configuration Example](docs/config.example.json)
- [Architecture](docs/architecture.md)
- [Performance Notes](docs/performance.md)
- [Plugin Guide](docs/plugins.md)
- [Testing Guide](docs/testing.md)
- [Troubleshooting](docs/troubleshooting.md)
- [Exit Codes](docs/exit-codes.md)
- [Roadmap](docs/roadmap.md)
- [Contributing](CONTRIBUTING.md)
- [Security Policy](SECURITY.md)

## Contributing

Contributions are welcome. Before submitting changes:

1. Build the project.
2. Run the test suite.
3. Run relevant quality checks when available.
4. Keep platform-specific behavior isolated where practical.
5. Include focused tests for behavior changes.

See [CONTRIBUTING.md](CONTRIBUTING.md) for repository-specific contribution guidance.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE).

---

<div align="center">

**devclean** · Developer cache visibility and cleanup without guesswork.

</div>
