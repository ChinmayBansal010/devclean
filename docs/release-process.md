# Release Process

## 1. Verify source state

Confirm `main` contains the intended feature set and that the working tree used for verification matches the branch.

## 2. Validate the build

Configure with the supported dependency/toolchain setup, build all targets, and run the complete CTest suite.

## 3. Validate quality gates

Run available sanitizer, formatting, Clang-Tidy, and Cppcheck checks. Platform CI must cover Linux, macOS, and Windows before a stable release.

## 4. Validate packaging

Build the configured archive/package formats and verify that the installed executable reports the expected version.

## 5. Publish

Update the changelog, create the Git tag from the verified commit, publish release artifacts, and record any known limitations.
