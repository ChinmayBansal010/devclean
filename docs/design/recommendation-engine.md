# Recommendation Engine

Recommendations translate scan observations into user-facing actions without performing cleanup automatically.

## Inputs

The engine can consider cache size, reclaimable bytes, safety state, activity state, age, growth, and configured exclusions.

## Outputs

A recommendation should include an action, confidence, rationale, and estimated recovery. Suggested actions include keep, inspect, clean, or clean-stale.

## Ordering

Recommendations should prioritize high-impact safe actions first. A very large cache should not outrank a smaller cache if the larger one is active or protected.

## Explainability

Every recommendation should be explainable from observable inputs. Avoid opaque scores that cannot tell the user why an item was selected.

## Non-destructive behavior

The recommendation engine is advisory. It must not call filesystem mutation APIs. The cleanup planner remains responsible for converting a recommendation into a validated execution plan.
