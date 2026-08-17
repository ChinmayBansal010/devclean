# Changelog

All notable changes to this project will be documented in this file.

## [1.2.0] - 2026-08-17

### 🚀 New Capabilities

- Added `devclean doctor` for developer-environment diagnostics.
- Added target-based cleanup with `--target`.
- Added stale-file cleanup with `--stale`.
- Added safe cleanup mode with `--safe`.
- Added `recommend` cache health scoring and ranked cleanup guidance.
- Added the interactive cache health `dashboard`.
- Added reusable storage budget calculations and deterministic cleanup planning.
- Added cache policy scoring for safety, activity, age, size, and growth.
- Added historical scan diffing and cache growth projection primitives.
- Added centralized path safety inspection and stale-file indexing.
- Added resilient directory size estimation for permission-heavy filesystems.
- Added cleanup operation manifests for future audit and recovery workflows.
- Added a normalized report model for stable output generation.
- Added developer environment summary structures.
- Added duplicate-file candidate grouping for future reclaimable-space analysis.

### 🧱 Engineering Improvements

- Centralized the release version at the CMake project definition and exposed it through `${PROJECT_VERSION}`.
- Expanded the reusable analysis layer under `src/engine`.
- Added CMake `CONFIGURE_DEPENDS` discovery for analysis engine sources.
- Preserved existing CLI behavior while introducing reusable analysis primitives.
- Kept filesystem safety decisions separate from destructive cleanup execution.

### 🔧 Fixes and Improvements

- Improved cross-platform cache path resolution and tool detection.
- Respected cache-specific environment variables when resolving configured paths.
- Optimized scanning by limiting work to requested cache definitions.
- Applied scan filters consistently across built-in, custom, and plugin caches.
- Fixed Windows debug scan failures and improved Windows path handling in CLI tests.
- Normalized Windows paths in tests to make filesystem comparisons reliable across path formats.

## [1.0.1] - 2026-08-08

### 🔧 Fixes and Improvements

- Improved cross-platform cache path resolution and tool detection.
- Respected cache-specific environment variables when resolving configured paths.
- Optimized scanning by limiting work to requested cache definitions.
- Applied scan filters consistently across built-in, custom, and plugin caches.
- Fixed Windows debug scan failures and improved Windows path handling in CLI tests.
- Normalized Windows paths in tests to make filesystem comparisons reliable across path formats.

## [1.0.0] - 2026-07-25

### 🎉 Initial Stable Release

The first stable release of **devclean**, a fast, cross-platform developer cache management utility designed to analyze, manage, and safely clean development caches.

### ✨ Features

- Cross-platform cache discovery and cleanup
- Support for major development ecosystems:
  - Python (pip, pipenv, Poetry, uv)
  - Rust (Cargo)
  - JavaScript (npm, pnpm, Yarn, Bun)
  - Java (Gradle, Maven)
  - C/C++ (ccache, Conan, vcpkg)
  - Containers (Docker, Podman)
  - Editors and IDEs
  - Build systems and package managers
- Historical cache tracking and storage analysis
- Intelligent cache recommendations based on usage
- Safe cache cleanup with interactive confirmation
- Multiple report formats:
  - JSON
  - Markdown
  - HTML
  - CSV
- Plugin system for custom cache definitions
- Configuration file support with automatic legacy migration

### 🛡️ Safety

- Protected system directory detection
- Symlink attack prevention
- Active process detection for in-use caches
- Dry-run mode for previewing cleanup operations
- Strict validation for configuration files and plugins
- Cross-platform filesystem protection

### 📦 Distribution

- **Linux**
  - DEB
  - RPM
  - TAR.GZ
- **Windows**
  - ZIP
  - Winget
  - Chocolatey
- **macOS**
  - Homebrew
  - TAR.GZ

Additional resources include:

- Bash shell completion
- Unix man pages

### ⚙️ CI/CD

- Automated multi-platform builds
- Linux, macOS, and Windows testing
- AddressSanitizer and UndefinedBehaviorSanitizer integration
- Static analysis with Clang-Tidy and Cppcheck
- Automated release packaging and publishing via GitHub Actions

### 📈 Project Highlights

- Modern C++23 implementation
- Cross-platform architecture
- Extensible plugin framework
- Comprehensive automated test suite
- Security-focused cleanup engine
- Open-source under the MIT License