# JSON Versioning

Machine-readable output is versioned independently from the application release.

Fields are added compatibly when possible. Removing or changing field meaning requires an output-version transition and release note. Consumers should ignore unknown fields and rely on stable identifiers rather than formatted messages.