# Contributing

## Workflow

1. Create a focused branch
2. Build in a fresh directory
3. Run tests before opening a pull request
4. Keep changes small and explain user-facing impact
5. Check the relevant documentation when changing CLI behavior

## Local checks

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
cmake --build build --target tidy
cmake --build build --target cppcheck
```

For a clean cross-platform check, use the same CMake configure/build/test flow on Linux, Windows, and macOS. The repository CI workflow runs this matrix for changes targeting `main`.

## Style

- Follow the existing modular boundaries
- Keep cache definitions registry-driven
- Route filesystem mutations through `platform/Filesystem`
- Prefer small, well-tested changes over broad rewrites
- Keep machine-readable output stable when changing command output
- Update CLI documentation when adding or changing options

## Pull requests

- Include motivation
- Note safety implications for cleanup changes
- Add or update tests when behavior changes
- Document user-visible CLI changes
- Confirm generated build and packaging artifacts are not included accidentally
