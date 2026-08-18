# Observability Model

Diagnostics and cleanup results should be observable without coupling core logic to a particular renderer.

## Events

Useful events include scan start and completion, cache discovery, candidate rejection, plan creation, cleanup start, cleanup result, and fatal command errors.

## Levels

Normal user output should remain concise. Verbose and debug modes can expose detailed reasons, timings, and platform errors. Machine-readable output should expose structured events or result fields rather than formatted log strings.

## Privacy

Logs should avoid dumping full environment variables, configuration contents, or arbitrary file names. Absolute paths should be included only when needed for actionable diagnostics.

## Performance

Instrumentation should have low overhead in normal operation. Timing collection can be enabled by default for aggregate phases while detailed per-file tracing remains opt-in.
