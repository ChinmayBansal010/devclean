# Security Review Checklist

Security review for devclean focuses on filesystem mutation, path resolution, configuration input, plugins, and generated reports.

## Filesystem

Verify that cleanup roots are validated, symlink behavior is understood, protected locations are rejected, and force mode cannot bypass path validation.

## Configuration

Treat configuration as untrusted input. Validate paths, durations, byte limits, and plugin metadata before use. Avoid shell evaluation of configured commands.

## Plugins

Reject incompatible plugin versions and isolate plugin failures. Plugins must not gain a separate path to filesystem mutation.

## Reports

Escape data when rendering HTML or other formats that interpret markup. Structured output should not accidentally expose secrets from environment variables or configuration files.

## Dependency hygiene

Keep third-party actions and libraries current, review major updates, and avoid granting workflow permissions that are not required by the job.
