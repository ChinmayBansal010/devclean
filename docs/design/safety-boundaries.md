# Safety Boundaries

Destructive operations require explicit boundaries.

The cleaner rejects filesystem roots, protected locations, unresolved paths, and paths that escape an allowed cache boundary after normalization. Dry-run uses the same planner as real cleanup and differs only at the final mutation step.

Safety decisions are recorded so a user can understand why a candidate was skipped.