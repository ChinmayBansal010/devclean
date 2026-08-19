# Safe Mode

Safe mode narrows cleanup to candidates that satisfy all configured safety checks.

Active tools, protected paths, ambiguous ownership, unresolved paths, and caches carrying explicit destructive warnings are excluded. Safe mode does not bypass confirmation and does not imply that every remaining candidate is risk-free.

The planner exposes the exclusions so users can understand the conservative result.