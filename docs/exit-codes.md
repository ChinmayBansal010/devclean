# Exit Codes

`devclean` uses conventional exit codes so shell scripts and CI jobs can distinguish success from failure.

| Code | Meaning |
| ---: | --- |
| `0` | Command completed successfully. |
| `1` | Invalid command, invalid operation, or operation failure. |
| `130` | The operation was interrupted by `SIGINT` or `SIGTERM`. |

For automation, treat any non-zero status as unsuccessful unless the workflow explicitly handles an interruption.

## Examples

```bash
devclean scan
status=$?

if [ "$status" -ne 0 ]; then
  echo "devclean failed with status $status"
  exit "$status"
fi
```

The exit status is intentionally independent from human-readable output so scripts do not need to parse terminal text.
