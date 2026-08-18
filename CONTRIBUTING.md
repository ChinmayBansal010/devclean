# Contributing

## Workflow

Create focused changes, build from a clean directory, run the relevant tests, and document user-facing behavior. For repository maintenance, keep automation changes separate from product behavior changes.

## Local checks

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
cmake --build build --target tidy
cmake --build build --target cppcheck
```

For a clean cross-platform check, use the same CMake configure/build/test flow on Linux, Windows, and macOS. The repository CI workflow runs the platform matrix for changes targeting `main`.

## Safety-sensitive code

Any code that can discover or delete files must preserve the established protected-path and process-awareness rules. Filesystem mutations should remain behind the platform and cleanup layers rather than being introduced directly in command handlers.

## CLI behavior

New options should have clear help text, deterministic exit codes, stable machine-readable output, and compatible composition with existing safety flags. Destructive commands should support a dry-run path where practical.

## Tests

Behavior changes should include focused regression coverage. Filesystem tests should use isolated temporary fixtures and explicitly cover failure, symlink, protected-path, and platform-specific cases when relevant.

## Pull requests

Include motivation, affected components, safety implications, test evidence, and user-visible CLI changes. Do not commit generated build directories, package artifacts, credentials, or local machine configuration.
