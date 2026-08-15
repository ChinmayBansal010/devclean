# Release checklist

- Update the version in the project metadata.
- Run a clean Debug build and CTest.
- Run a clean Release build and CTest.
- Exercise `--version`, `--help`, scan, stats, analyze, and dry-run clean.
- Build the CPack package.
- Review the changelog for user-visible changes.
