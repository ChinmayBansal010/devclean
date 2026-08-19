# Target Cleanup

Target cleanup accepts a byte budget and chooses candidates until the budget is reached or no safe candidates remain.

The planner must never exceed the requested target because of an arithmetic overflow or stale measurement. Candidate sizes are bounded before accumulation. If the exact target cannot be reached, the result reports the selected bytes and remaining gap.

Dry-run and real cleanup share the same selection plan.