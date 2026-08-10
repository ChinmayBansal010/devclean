# Release Checklist

Use this checklist before tagging a devclean release.

## Validation

- [ ] Configure a clean build directory.
- [ ] Build in Debug and Release configurations.
- [ ] Run the complete CTest suite.
- [ ] Run clang-tidy and cppcheck when available.
- [ ] Verify the CLI on Linux, Windows, and macOS where release artifacts are supported.
- [ ] Exercise `scan`, `analyze`, `stats`, `clean --dry-run`, and `version`.
- [ ] Verify protected-path cleanup remains blocked.

## Documentation

- [ ] Update the version and release date.
- [ ] Update the changelog with user-visible changes.
- [ ] Review CLI examples for stale options.
- [ ] Review packaging and installation instructions.

## Packaging

- [ ] Build the configured CPack packages.
- [ ] Verify package contents and executable permissions.
- [ ] Confirm generated artifacts are not accidentally committed outside the release workflow.
- [ ] Create the Git tag only after the final validation passes.
