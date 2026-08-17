# Analysis Engine Architecture

The analysis engine is the reusable layer beneath future `devclean` commands. It keeps storage policy, historical comparison, filesystem safety, and reporting concerns independent from the CLI dispatcher.

## Pipeline

```text
ScanResult[]
    |
    +--> CachePolicy --------> cleanup priority
    |
    +--> ScanDiff -----------> historical change set
    |
    +--> GrowthModel --------> projection and acceleration
    |
    +--> StorageBudget ------> budget pressure
    |
    +--> CleanupPlanner -----> target-aware candidate plan
    |
    +--> ReportModel --------> normalized report rows
```

## Safety boundary

Filesystem-sensitive operations should pass through `PathSafety` before a destructive operation is scheduled. The safety layer rejects protected locations, symbolic links that require review, and paths that cannot be verified.

The planner does not delete files. It produces a deterministic plan containing the cache name, reason, byte estimate, priority, and safety state. This separation allows `--dry-run`, interactive cleanup, reports, and future automation to consume the same plan.

## Historical analysis

`ScanDiff` compares named cache results and records byte and file deltas. `GrowthModel` consumes recent deltas to estimate a daily rate, a thirty-day projection, and whether growth is accelerating.

## Reporting

`ReportModel` converts scan results into a stable row-oriented representation. Renderers can consume that model without needing to know scanner internals.

## Extension strategy

The reusable engine sources live under `src/engine` and are discovered by CMake with `CONFIGURE_DEPENDS`. New analysis primitives should remain small, deterministic, and free of CLI concerns. Command-specific formatting belongs in command or formatter code.

## Design constraints

1. Never treat an estimate as permission to delete.
2. Never bypass protected-path checks because a cache is large.
3. Keep historical calculations deterministic for identical input.
4. Prefer explicit safety state over implicit assumptions.
5. Keep reporting structures independent of terminal formatting.
6. Preserve existing CLI behavior when adding analysis capabilities.