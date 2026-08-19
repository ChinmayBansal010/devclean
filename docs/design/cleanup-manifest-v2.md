# Cleanup Manifest Contract

A cleanup manifest records planned operations before mutation.

Each entry contains a normalized path, cache identifier, expected size, policy decision, and reason. Dry-run output is generated from the same manifest used by execution. The executor validates each path again immediately before mutation because filesystem state can change between planning and execution.

A manifest describes intent; it is not a permission token.