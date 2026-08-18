# Plugin Contract

Plugins extend cache discovery without requiring the core registry to know every ecosystem.

A plugin should provide a stable cache identity, category, candidate paths, and cleanup policy metadata. Plugin-provided paths must pass the same filesystem safety checks as built-in definitions.

## Compatibility

Plugins should avoid depending on internal implementation classes. The public contract should remain small enough that new scanner and reporting features do not require plugin rewrites.

## Failure isolation

A malformed or unavailable plugin should be reported as a plugin error and must not prevent built-in cache discovery.

## Security

Plugin data is untrusted configuration. Paths, identifiers, and optional metadata must be validated before entering the scan or cleanup pipeline.
