# Build presets

The repository provides `debug` and `release` CMake presets.

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Release builds use the same vcpkg toolchain:

```bash
cmake --preset release
cmake --build --preset release
ctest --preset release
```
