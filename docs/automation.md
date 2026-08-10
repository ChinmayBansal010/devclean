# Automation Guide

`devclean` is intended to be usable from shell scripts and CI without parsing terminal-oriented text.

## Machine-readable commands

Prefer JSON output when a downstream tool consumes the result:

```bash
devclean scan --json
devclean stats --json
devclean analyze --report json
```

## Safe cleanup automation

Preview cleanup before allowing a scheduled job to delete cache data:

```bash
devclean clean --dry-run
```

For unattended cleanup, use an explicit target and exclusions rather than relying on a broad default selection:

```bash
devclean clean --force python --exclude poetry
```

## CI integration

A typical CI step should build and test devclean before using it:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Use the process exit code as the primary success signal. See [Exit Codes](exit-codes.md) for the supported statuses.

## Stability rule

Automation should depend on documented JSON fields and exit codes, not on human-readable headings or spacing. This keeps scripts resilient when terminal presentation improves.
