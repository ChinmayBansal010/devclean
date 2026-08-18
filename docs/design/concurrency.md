# Concurrency Guidelines

Scanning and cleanup can be expensive because directory traversal is dominated by filesystem latency. Concurrency should improve throughput without changing correctness.

## Scanning

Independent cache roots may be scanned concurrently. Shared mutable state should be avoided or protected, and results should be collected before deterministic ordering is applied.

## Cleanup

Cleanup concurrency is more restrictive. Two operations must never mutate the same cache root concurrently, and the final filesystem safety check should occur close to mutation time.

## Resource limits

Unbounded worker creation is inappropriate for large developer environments. A bounded worker pool should be used when concurrency is introduced, with the default sized conservatively for the host.

## Error handling

A failure in one worker should be recorded against that cache rather than terminating unrelated work. Fatal cancellation should still be possible when the caller explicitly requests it.
