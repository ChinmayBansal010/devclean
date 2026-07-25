# Security Policy

## Supported versions

The latest `main` branch and the latest tagged release receive security fixes.

## Reporting

Please report suspected vulnerabilities privately to the maintainers before opening a public issue.

Include:

- affected version or commit
- operating system
- reproduction steps
- expected and actual behavior

## Project safeguards

`devclean` is designed to:

- refuse deletion of protected directories
- reject unsafe plugin paths
- reject unsafe custom cache paths
- warn about active tools before cleanup
