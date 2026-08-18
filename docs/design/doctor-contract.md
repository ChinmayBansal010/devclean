# Doctor Command Contract

`devclean doctor` is a diagnostic command, not a cleanup command. It may inspect executables, versions, environment variables, permissions, and relevant project directories, but it must not modify the user's environment.

## Result categories

Each diagnostic should resolve to one of: available, unavailable, degraded, or unknown. Unknown is preferable to reporting a guessed state when a platform-specific check cannot be performed.

## Machine-readable output

JSON output should preserve stable field names and use explicit booleans only where the underlying check is definitive. Human-readable output can add explanatory text without changing the structured contract.

## Exit behavior

A diagnostic finding is not automatically a command failure. The command should reserve a non-zero exit status for an inability to complete the diagnostic operation itself. Individual missing tools are findings that belong in the report.

## Safety

Doctor must not invoke package managers, start services, remove caches, or modify shell configuration. Recommendations are informational only.
