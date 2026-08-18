# Security Model

`devclean` performs filesystem operations with potentially destructive consequences, so safety is a core architectural concern.

## Trust boundaries

Cache definitions, plugin metadata, configuration files, and filesystem paths should be treated as untrusted inputs.

## Path validation

Before deletion, a candidate must be normalized and checked against protected locations. Symbolic links and path traversal must not be allowed to redirect cleanup outside the intended cache root.

## Process awareness

A cache associated with an active development tool should be treated conservatively. Safe mode should reject uncertain active-state decisions rather than guessing.

## Auditability

Cleanup manifests and structured reports should make it possible to understand what was planned, what was executed, and what was skipped.
