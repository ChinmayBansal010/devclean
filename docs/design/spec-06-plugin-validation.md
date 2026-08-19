# Plugin Validation

Plugin definitions must provide a stable identifier, display name, category, and one or more cache locations.

Paths are normalized before registration. Invalid definitions are rejected with a diagnostic that identifies the plugin and field. Validation is performed before the definition enters the shared registry.