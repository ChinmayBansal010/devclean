# Report Schema Versioning

JSON reports are an interface for scripts as well as humans. Their schema therefore needs an explicit version.

Every machine-readable report should identify its schema version independently of the application version. Additive fields may be introduced without changing the major schema version when existing fields retain their meaning. Removing or changing the type or semantics of an existing field requires a breaking schema revision.

Renderers must not parse human-formatted output to produce JSON. Structured results should flow directly from the core model to the selected renderer.
