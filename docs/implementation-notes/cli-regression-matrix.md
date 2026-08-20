# CLI Regression Matrix

CLI regression coverage should include command-only invocation, help and version flags, missing option values, invalid values, repeated options, positional targets, and machine-readable output.

The matrix should be run after parser changes because command-line compatibility is easy to break while adding new flags.
