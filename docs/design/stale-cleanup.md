# Stale Cleanup

Stale cleanup evaluates individual files against an age threshold where the cache policy permits file-level removal.

The threshold is measured from filesystem modification time. Missing timestamps, inaccessible files, and protected entries are excluded rather than guessed. Directory deletion is never implied by the age of one child.

The report records the threshold and number of eligible files.