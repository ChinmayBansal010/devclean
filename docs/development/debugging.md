# Debugging

For a debug build:

```bash
cmake --preset debug
cmake --build --preset debug
```

Run a command directly under a debugger, or enable CMake compiler debug symbols through the Debug configuration. For scanner issues, first reproduce with `scan --json` and record the detected path, size, and category before changing code.
