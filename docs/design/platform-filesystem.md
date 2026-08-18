# Platform Filesystem Boundary

Platform-specific filesystem behavior belongs behind the `platform/Filesystem` boundary. Core planning code should operate on normalized paths and metadata without embedding operating-system APIs.

## Responsibilities

The platform layer owns path normalization, existence checks, metadata retrieval, directory traversal, removal, and platform-specific error translation.

## Core assumptions

Core code may assume that a normalized path is syntactically valid, but it must still treat filesystem operations as fallible. Permissions can change between scanning and cleanup.

## Path identity

Comparisons should use normalized representations appropriate for the host. A lexical comparison alone is insufficient when symlinks or platform-specific case rules can alter the resolved target.

## Testing

Platform adapters should expose enough deterministic behavior to test safety and planning logic without requiring a real system cache. Integration tests should still exercise the native implementation on supported platforms.
