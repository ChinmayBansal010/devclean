# Plugin Compatibility

Cache-provider plugins are an extension boundary and must not be allowed to destabilize the scanner.

Plugins should declare a compatibility contract containing the provider identity, supported API version, cache definitions, and capabilities. The loader rejects unsupported API versions before invoking provider logic.

A malformed or failing plugin is isolated from built-in providers. Discovery continues for valid providers and the failure is surfaced as a diagnostic event.

Plugin output is normalized into the same cache model used by built-in definitions so downstream scanning, analysis, reporting, and cleanup do not need plugin-specific branches.
