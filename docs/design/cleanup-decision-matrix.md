# Cleanup Decision Matrix

Cleanup candidates are evaluated in a deterministic order so interactive, target-based, stale, and safe cleanup produce explainable results.

## Decision inputs

- cache activity state
- safety classification
- staleness age
- reclaimable bytes
- cleanup risk
- user-selected target
- explicit cache filters

## Ordering

1. Reject protected or unsafe candidates.
2. Reject active candidates in safe mode.
3. Apply explicit cache filters.
4. Apply stale-file constraints when requested.
5. Rank remaining candidates by reclaimable space and policy score.
6. Stop when the requested target is reached.

A candidate must never be selected solely because it is large. The planner must preserve the safety and policy decisions produced earlier in the pipeline.
