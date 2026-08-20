# Build Reproducibility

DevClean builds with C++23 and CMake. Reproducible builds should use the repository CMake presets, a clean build directory, and the pinned dependency configuration rather than relying on machine-specific compiler flags.

## Rules

- Prefer `CMakePresets.json` for local and CI configuration.
- Keep optional diagnostics such as sanitizers behind explicit CMake options.
- Do not commit generated build directories or package staging trees.
- Record toolchain changes in the changelog when they affect supported environments.
