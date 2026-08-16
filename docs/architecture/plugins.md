# Plugin architecture

Plugins extend cache discovery without changing the built-in registry.

A plugin definition provides a cache name, aliases, category, platform paths, environment variables, and enablement state. The scanner loads plugin definitions, applies command filters, and merges them with built-in definitions.

Plugins must be treated as untrusted input. Invalid definitions should be rejected, malformed paths should not escape configured boundaries, and a plugin failure should not prevent built-in caches from being scanned.
