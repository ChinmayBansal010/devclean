# Changelog

All notable changes to this project will be documented in this file.

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