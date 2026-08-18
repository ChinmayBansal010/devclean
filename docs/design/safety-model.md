# Cleanup Safety Model

Safety is a property of the complete operation, not just a command-line flag.

## Layers

1. Path validation establishes that a candidate is inside a configured cache root.
2. Cache policy determines whether the cache is eligible for mutation.
3. Runtime checks detect active tools and protected resources.
4. The cleanup planner produces an explicit candidate set.
5. The filesystem layer performs the final mutation and reports failures.

Each layer can reject a candidate. A later layer must never override an earlier safety decision.

## Protected locations

System roots, home-directory roots, and unresolved paths are rejected unless a cache definition explicitly establishes a safe root. Relative paths should be resolved before validation.

## Symlinks

Symlink traversal requires special care because the apparent path and resolved target may differ. Cleanup should validate the resolved location before recursive mutation.

## Force mode

Force should relax confirmation policy, not path-safety invariants. A force flag must never turn an invalid or unresolved path into an eligible deletion target.
