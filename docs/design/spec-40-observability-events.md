# Observability Events

Core operations expose structured lifecycle events for scan start, cache discovery, measurement, plan creation, cleanup completion, and fatal errors.

Events carry stable identifiers and aggregate timings. They do not expose secrets or unrestricted environment state. Renderers and future telemetry adapters consume these events without changing command behavior.