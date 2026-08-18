# Cleanup Manifest

A cleanup manifest is a record of what the planner intended to mutate and what the filesystem actually changed.

## Planned state

The manifest should record cache name, candidate path, reason for selection, estimated bytes, safety classification, and the operation mode.

## Result state

Each planned item should resolve to removed, skipped, failed, or changed-before-execution. Actual recovered bytes should be measured where practical rather than inferred solely from the scan.

## Reproducibility

A dry-run manifest should be serializable and suitable for review before executing the same plan. Execution must revalidate paths because filesystem state can change between planning and mutation.

## Privacy

Manifests should avoid recording file contents and should not expose unrelated personal paths when a stable cache identifier is sufficient. Absolute paths are useful for debugging but should be optional in exported reports.
