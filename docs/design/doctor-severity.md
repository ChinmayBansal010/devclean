# Doctor Severity

Doctor findings use `pass`, `info`, `warning`, and `error` severities.

Severity reflects operational impact, not whether a tool is installed. Optional tooling may be absent without being an error. Human output groups findings by severity; JSON preserves the original check identifier, severity, and message.

Exit status is non-zero only when configured fatal checks fail.