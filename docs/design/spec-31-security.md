# Security Model

Security-sensitive behavior is concentrated around path handling, configuration loading, plugin loading, and cleanup execution.

Untrusted strings are treated as data. Paths are normalized before policy checks, reports escape rendered content, and plugins cannot bypass cleanup policy through registration metadata.