# History Diff Contract

History comparisons align snapshots by stable cache identity rather than list position.

The diff reports added, removed, resized, and unchanged caches. Identity changes remain visible instead of being silently merged. Missing historical fields are treated as unknown rather than fabricated.

The comparison layer is read-only and does not mutate stored history.