# Doctor Diagnostics

`devclean doctor` should distinguish discovery, health, and actionability.

## Diagnostic states

- `available`: tool was detected and can be inspected
- `missing`: expected tool was not found
- `inactive`: tool exists but no relevant activity was detected
- `warning`: inspection produced a condition that may affect cleanup
- `error`: the diagnostic could not be completed

Diagnostics must remain non-destructive. Running `doctor` must never delete caches, modify configuration, terminate processes, or change the environment.

JSON output should preserve the state, tool identity, version when available, and a stable diagnostic code.
