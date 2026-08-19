# Exit Codes

Exit codes distinguish successful execution, usage errors, runtime failures, and policy-rejected operations.

Output text must never be used as a machine-readable success signal. JSON output carries detailed reasons while the process status remains stable for scripts and CI.