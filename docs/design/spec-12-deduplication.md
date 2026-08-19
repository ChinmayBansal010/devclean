# Deduplication

Duplicate filesystem paths are collapsed after normalization before planning cleanup.

The same physical path may be referenced by multiple cache definitions. The planner must avoid scheduling it twice and should retain all contributing cache identifiers for reporting. Deduplication is descriptive and does not alter cache registry definitions.