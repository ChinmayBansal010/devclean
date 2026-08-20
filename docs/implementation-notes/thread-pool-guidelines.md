# Thread Pool Guidelines

Concurrent scanning should separate filesystem work from result aggregation and avoid unbounded task creation.

Workers should observe cancellation, propagate exceptions to the owning operation, and shut down cleanly before command completion. Concurrency must not change cleanup eligibility or report semantics.
