# Troubleshooting

## No caches are found

Run `devclean scan --verbose` to inspect resolved cache paths and warnings. If a tool stores its cache in a non-standard location, configure the path through the environment variable supported by that tool or add a custom cache definition to the devclean configuration.

## Cleanup is refused

Protected locations and paths that resolve outside the configured cache scope are rejected by the safety layer. Use `devclean clean --dry-run` first to inspect the selected paths.

If a cache is intentionally managed elsewhere, add an explicit custom cache rather than weakening the protected-path checks.

## JSON output is needed in a script

Use `--json` with commands that support machine-readable output:

```bash
devclean scan --json
devclean stats --json
devclean analyze --report json
```

Keep human-readable output for interactive use and JSON output for automation.

## Tests fail after a dependency update

Start from a clean build directory and rebuild the project:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On Windows, remove the build directory from the shell or IDE before reconfiguring CMake.

## Need more detail

Run `devclean --help` for the current command and option list. The repository also contains the [CLI Reference](cli.md) and [Testing Guide](testing.md).
