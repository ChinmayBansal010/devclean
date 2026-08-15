# JSON output

Use `--json` when devclean is consumed by scripts or other tools. JSON output should be preferred over parsing human-readable tables because field names remain machine-readable.

Example:

```bash
devclean scan --json > scan.json
```
