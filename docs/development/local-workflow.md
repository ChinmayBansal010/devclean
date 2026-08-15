# Local workflow

1. Configure with `cmake --preset debug`.
2. Build with `cmake --build --preset debug`.
3. Run `ctest --preset debug`.
4. Exercise the CLI through `scripts/run.sh`.
5. Keep cleanup changes covered by tests and path-safety checks.
