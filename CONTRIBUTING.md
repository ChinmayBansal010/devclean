# Contributing

## Workflow

1. Create a focused branch
2. Build in a fresh directory
3. Run tests before opening a pull request
4. Keep changes small and explain user-facing impact

## Local checks

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
cmake --build build --target tidy
cmake --build build --target cppcheck
```

## Style

- Follow the existing modular boundaries
- Keep cache definitions registry-driven
- Route filesystem mutations through `platform/Filesystem`
- Prefer small, well-tested changes over broad rewrites

## Pull requests

- Include motivation
- Note safety implications for cleanup changes
- Add or update tests when behavior changes
