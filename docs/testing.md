# Testing Guide

## Configure

The project uses CMake and CTest. A vcpkg checkout is included for reproducible dependency setup.

```bash
./vcpkg/bootstrap-vcpkg.sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake
```

On Windows, use `vcpkg/bootstrap-vcpkg.bat` and a CMake generator supported by the installed compiler.

## Build and test

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The CLI test executable covers argument parsing, cache registry matching, configuration migration, environment-driven cache paths, plugin loading, protected-path checks, and destructive-cleanup behavior.

## Sanitizers

Enable AddressSanitizer and UndefinedBehaviorSanitizer on supported toolchains:

```bash
cmake -S . -B build-sanitize -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_SANITIZERS=ON \
  -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build-sanitize --parallel
ctest --test-dir build-sanitize --output-on-failure
```

## Static analysis

When the tools are installed, the repository exposes CMake targets for clang-tidy, cppcheck, and formatting:

```bash
cmake --build build --target tidy
cmake --build build --target cppcheck
cmake --build build --target format
```

CI runs the test suite on Linux, macOS, and Windows and also runs sanitizer and static-analysis jobs.
