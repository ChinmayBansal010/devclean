# Dry-Run Review

A dry run should answer three questions before a cleanup is executed: what will change, why each item qualifies, and how much storage is expected to be reclaimed.

For automation, the dry-run result should be stable enough to compare between scans and should not mutate the filesystem.
