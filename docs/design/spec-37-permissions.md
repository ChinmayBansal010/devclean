# Permission Policy

Read-only scans should continue when individual cache entries are inaccessible, while cleanup requires mutation permission for each selected path.

Permission failures are recorded with the affected cache and operation. The cleaner must not elevate privileges or silently skip safety checks to obtain access.