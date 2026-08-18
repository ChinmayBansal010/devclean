# Configuration Precedence

Configuration should be predictable when values come from built-in defaults, user configuration, environment variables, and command-line arguments.

## Precedence

The recommended order is:

1. Built-in defaults
2. User configuration
3. Environment overrides
4. Explicit command-line arguments

A more specific source wins over a less specific source.

## Merge behavior

Scalar settings replace earlier values. Lists should define whether they replace or extend the existing list; silent mixing is discouraged. Cache definitions should be merged by stable cache name so overrides can modify one definition without duplicating it.

## Validation

Configuration is validated after precedence has been resolved. This ensures a command-line override can repair an invalid default only when the resulting final configuration is valid.

## Reporting

Doctor output should be able to explain where an effective configuration value came from when troubleshooting configuration behavior.
