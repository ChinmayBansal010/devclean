# Recovery Semantics

Cleanup execution must distinguish planning failure, execution failure, and interruption.

A cleanup operation should record each attempted action and its result. Successful deletions are retained in the operation manifest even when a later deletion fails.

An interrupted cleanup must stop scheduling new destructive work, finish only the minimum state needed to produce a consistent manifest, and report partial completion. It must never claim that the requested target was fully recovered unless the measured results support that claim.

Recovery metadata is diagnostic information. It must not imply that deleted files can be restored unless an explicit backup mechanism exists.
