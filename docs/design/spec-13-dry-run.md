# Dry Run

Dry-run produces the complete cleanup plan without filesystem mutation.

It uses the same filtering, safety, target, and stale policies as execution. Any difference between dry-run and execution should be treated as a defect unless the filesystem changed between planning and mutation.