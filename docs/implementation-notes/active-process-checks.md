# Active Process Checks

A cleanup candidate can be valid by age or size and still be unsafe to remove while an associated process is active.

Active-process checks should be performed before mutation and should fail closed when the safety policy cannot establish that a candidate is safe.
