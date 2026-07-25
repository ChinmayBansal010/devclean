# Changelog

## 1.0.0 - 2026-07-25

Initial stable release.

### Features
- Scan developer cache directories across Python, Rust, JavaScript, Java, C++, containers, editors, build tools, and package managers
- Analyze cache growth with historical tracking and recommendations
- Clean discovered cache directories with safety checks
- Statistics and reporting (JSON, Markdown, HTML, CSV)
- Plugin system for custom cache definitions
- Config file support with legacy migration

### Safety
- Protected path detection prevents deletion of system directories
- Symlink attack detection
- Active tool detection warns before deleting in-use caches
- Dry-run mode for safe preview
- Config and plugin input validation

### Packaging
- Linux: DEB, RPM, TGZ
- Windows: ZIP, Winget, Chocolatey
- macOS: Homebrew, TGZ
- Bash completion
- Man page

### CI/CD
- GitHub Actions: build, test, sanitizers, static analysis
- Automated release packaging on tags