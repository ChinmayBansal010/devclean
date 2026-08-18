# Cleanup Policy

`devclean` treats cleanup as a policy decision rather than a blind filesystem deletion.

## Selection order

1. Reject protected or unsafe paths.
2. Exclude actively used caches when activity detection is available.
3. Apply explicit user filters such as age and target size.
4. Prefer stale and low-risk entries before active or high-risk entries.
5. Produce a deterministic cleanup plan before execution.

## Target cleanup

`--target` is an upper recovery goal, not permission to exceed the requested amount. The planner should stop once the target is satisfied or no additional eligible entries remain.

## Safe mode

`--safe` narrows the candidate set by rejecting active, protected, or otherwise uncertain entries. It should be suitable for unattended previews and conservative maintenance workflows.

## Dry runs

Dry runs must never mutate the filesystem. They should expose the same candidate selection and estimated recovery calculations used by an actual cleanup.
