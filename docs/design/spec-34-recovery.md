# Recovery Behavior

Cleanup failures are reported per operation so one inaccessible path does not hide successful work on unrelated candidates.

The final result distinguishes planned, completed, skipped, and failed operations. A failed deletion is never retried blindly within the same command.