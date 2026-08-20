# Large Scan Operations

Large directory trees should be processed incrementally where possible so memory use does not scale with the entire tree.

Prefer bounded queues, incremental result handling, and explicit cancellation points. Any optimization must preserve cleanup safety and deterministic reporting.
