# History Retention

Scan history should remain useful without growing without bound.

Retention policy should be explicit and deterministic. A history record contains enough information to compare cache size, activity, and reclaimability across observations. Records must be ordered by observation time and identified by a stable schema version.

Retention cleanup must not affect cache cleanup. Removing old history records is a metadata operation and must never delete files from developer caches.

Future retention controls should report how many records will be removed before changing persistent history.
