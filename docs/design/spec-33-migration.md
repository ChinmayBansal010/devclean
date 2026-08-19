# Configuration Migration

Configuration changes that rename or remove fields should provide explicit migration diagnostics.

Unknown fields remain visible instead of being silently discarded when they may indicate a typo. Migration logic should be deterministic and should never perform destructive filesystem operations.