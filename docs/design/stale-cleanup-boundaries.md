# Stale Cleanup Boundaries

Stale cleanup operates on file age and must not infer that an entire cache is stale because its root directory is old.

The age check is applied to eligible files using their filesystem modification time. Directories are traversed only to discover files and are never removed solely because they exceed the age threshold.

If a timestamp cannot be read reliably, the file is excluded from stale cleanup and the reason is reported. This conservative behavior prevents metadata failures from becoming destructive decisions.
