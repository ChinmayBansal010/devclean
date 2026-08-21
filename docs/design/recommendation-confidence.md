# Recommendation Confidence

Recommendations should expose confidence instead of presenting heuristics as certainty.

## Levels

- **High:** strong evidence from activity, safety, age, and reclaimable size.
- **Medium:** useful evidence exists but one or more signals are incomplete.
- **Low:** the recommendation depends mostly on defaults or incomplete filesystem data.

Confidence describes the quality of the recommendation, not the cleanup risk. A high-confidence recommendation can still be high-risk and must remain blocked by safety policy.

## Output

Machine-readable output should expose both `recommendation` and `confidence`. Human-readable output should include a short reason explaining the strongest signals.
