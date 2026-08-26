<div align="center">

# devclean

**A cross-platform developer cache analyzer and cleanup CLI built with C++23.**

[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-555555?style=flat-square)](#platform-support)
[![License](https://img.shields.io/badge/license-MIT-2ea44f?style=flat-square)](LICENSE)

</div>

---

## What it does

`devclean` scans developer caches, measures storage usage, analyzes growth and health, diagnoses common development tooling, and plans cleanup with explicit safety constraints.

It supports registry-driven cache discovery, plugin/config extensions, scan history, target-based cleanup, stale-file cleanup, safe cleanup mode, structured reports, JSON output, and a terminal dashboard.

## Highlights

| Capability | What it provides |
|---|---|
| Cache scanning | Discover cache paths, sizes, files, directories, warnings, and categories |
| Analysis | Analyze storage usage and export structured reports |
| Health insights | Growth trends, health scoring, and ranked recommendations |
| Target cleanup | Clean toward a requested amount such as `10GB` |
| Stale cleanup | Remove files older than a duration such as `30d` |
| Safe mode | Exclude active, warned, and protected locations |
| Diagnostics | `devclean doctor` checks common developer tools |
| Dashboard | Interactive terminal view of cache health and recommendations |
| JSON output | Integrate commands into scripts and automation |
| Plugins | Extend the cache catalog without hard-coding every tool |
| History | Persist scan information for trend analysis |
| Cross-platform | Linux, macOS, and Windows path handling and packaging |

## Commands

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
./build/devclean scan
```

Inspect recommendations before cleaning:

```bash
./build/devclean doctor
./build/devclean recommend
./build/devclean clean --dry-run
```

Then apply safety constraints explicitly:

```bash
devclean clean --safe --target 10GB
devclean clean --safe --stale 30d
```

## Usage

### Scan

```bash
devclean scan
devclean scan --category python
devclean scan --json
devclean scan --active-only
```

### Analyze and report

```bash
devclean analyze
devclean analyze --report json
devclean analyze --report html
```

### Recommendations and dashboard

```bash
devclean recommend
devclean dashboard
```

### Cleanup

```bash
devclean clean --dry-run
devclean clean --safe
devclean clean --safe --target 10GB
devclean clean --safe --stale 30d
```

`--target` limits the cleanup budget. `--stale` applies file-age based cleanup. `--safe` excludes active, warned, and protected locations.

Supported duration units are `s`, `m`, `h`, `d`, and `w`.

### Diagnostics

```bash
devclean doctor
devclean doctor --json
devclean stats --json
```

JSON output is intended for automation, scripting, CI diagnostics, and other tooling.

## Supported caches

| Category | Examples |
|---|---|
| Python | `pip`, `pipenv`, `poetry`, `uv`, `pixi`, `mamba` |
| JavaScript | `npm`, `pnpm`, `yarn`, `bun`, `nvm` |
| Rust | `cargo`, `rustup` |
| Java | `gradle`, `maven` |
| C/C++ | `ccache`, `conan`, `vcpkg`, `cmake`, `meson`, `bazel` |
| Containers | Docker builder/volumes, `podman` |
| Editors / IDEs | VS Code, JetBrains tooling |

## Safety model

Cleanup is more restrictive than scanning. Protected filesystem locations are rejected, and safe mode adds checks for active tools, warnings, and protected paths.

Examples of protected locations include:

- `/`
- home directories
- Windows system roots
- `Program Files`

A recommended workflow is:

```text
SCAN → ANALYZE → REVIEW → DRY RUN → SAFE CLEANUP
```

## Platform support

- **Linux:** common cache roots under `~/.cache` and environment-defined locations.
- **macOS:** common tool/application caches under `~/Library/Caches`.
- **Windows:** `%USERPROFILE%`, `%LOCALAPPDATA%`, and `%APPDATA%` where applicable.

The project also contains cross-platform build and packaging workflows.

## Configuration

Configuration is loaded from:

- Linux/macOS: `~/.config/devclean/config.json`
- Windows: `%APPDATA%/devclean/config.json`

See [`docs/config.example.json`](docs/config.example.json) for an example configuration.

Useful fields include:

- `disabledCaches`
- `ignoredCaches`
- `defaultSort`
- `defaultCategory`
- `customCaches`

## Build with vcpkg

The repository includes a vcpkg checkout and root manifest.

```bash
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Development

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Optional tooling:

```bash
cmake --build build --target tidy
cmake --build build --target cppcheck
cmake --build build --target format
```

Sanitizers:

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
src/commands/  CLI commands
src/engine/    Cache analysis engines
src/scanner/   Discovery, registry, and plugins
src/cleaner/   Cleanup execution
src/platform/  Operating-system-specific behavior
src/utils/     Formatting, strings, terminal, and concurrency utilities
tests/         CLI, unit, and focused tests
cmake/         Warning and sanitizer configuration
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

Before submitting changes, build the project and run the relevant tests. Platform-specific changes should remain isolated where practical, and behavioral changes should include focused coverage.

See [CONTRIBUTING.md](CONTRIBUTING.md) for repository contribution guidance.

## License

MIT. See [LICENSE](LICENSE).

---

<div align="center">

**devclean** · Developer cache visibility and cleanup without guesswork.

</div>
