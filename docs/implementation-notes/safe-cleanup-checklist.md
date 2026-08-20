# Safe Cleanup Checklist

Cleanup should be treated as a planned operation rather than an unconditional recursive delete.

- Resolve and normalize the target path.
- Apply active-process and protected-path checks.
- Build a cleanup manifest before mutation.
- Prefer dry-run inspection for unfamiliar targets.
- Require explicit confirmation for destructive operations when configured.
- Record failures without silently converting them into successful removals.
