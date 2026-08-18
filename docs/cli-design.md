# CLI Design Principles

The command line interface should remain composable and predictable.

## Output modes

Human-readable output is optimized for interactive use. JSON is intended for automation. A command should not mix progress text into JSON output.

## Exit status

Successful scans and cleanups should use a zero exit status. Invalid arguments, configuration failures, or unrecoverable command errors should return non-zero status. Partial cleanup should remain distinguishable from a fully successful operation.

## Safety flags

Flags that reduce cleanup scope should compose cleanly. `--safe`, `--stale`, and `--target` should narrow or constrain the operation rather than silently overriding one another.

## Errors

Errors should identify the command, relevant path or input where safe to expose, and an actionable reason without leaking unnecessary filesystem details.
