# Concurrency Model

Independent cache measurements may run concurrently, but cleanup mutations must be coordinated.

Shared history and reporting state should be updated after worker results are collected. Concurrency must not change candidate ordering, safety decisions, or final output. A deterministic sort is applied after parallel measurement.