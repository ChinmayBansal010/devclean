# Target Cleanup Guarantees

`--target` expresses an upper bound on planned reclamation, not a promise that the exact number of bytes can always be recovered.

The planner should:

- never intentionally exceed the requested target
- report the estimated recovery before destructive work
- preserve whole-file semantics
- stop once the target is reached
- report when the available safe candidates cannot satisfy the target

Actual reclaimed bytes may differ from estimates because files can disappear, change, or become inaccessible between planning and execution.
