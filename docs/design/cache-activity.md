# Cache Activity Model

Activity detection determines whether a cache should be treated cautiously during cleanup.

## Signals

Useful signals include a running tool process, recent modification time, lock files, and cache-specific metadata. No single signal should be treated as universally authoritative across tools.

## Conservative behavior

When a cache is clearly active, safe cleanup should exclude it. When activity cannot be determined, the cache should remain eligible only under the normal policy and should be surfaced as uncertain rather than falsely marked safe.

## Tool mapping

Cache definitions should identify the executable or process family associated with the cache when active-process detection is supported. The mapping belongs in configuration or registry metadata rather than hardcoded into the cleanup engine.

## Reporting

Activity state should be visible in scan and recommendation output so users understand why a large cache was retained.
