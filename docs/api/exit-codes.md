# Exit codes

`devclean` uses non-zero exit codes for command or validation failures.

| Code | Meaning |
| --- | --- |
| 0 | Command completed successfully |
| 1 | General command failure |
| 2 | Invalid command-line arguments |
| 3 | Configuration could not be loaded or saved |
| 4 | Requested operation was interrupted or could not safely complete |

JSON output is intended for successful machine-readable command results. A caller should always inspect the process exit code before consuming output as authoritative.
