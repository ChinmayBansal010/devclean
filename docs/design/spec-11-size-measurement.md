# Size Measurement

Reported size should distinguish apparent bytes from reclaimable bytes where filesystem semantics make that distinction useful.

Measurement failures remain explicit. Integer arithmetic uses bounded unsigned operations and avoids overflow when aggregating large trees. Results are stable regardless of traversal order.