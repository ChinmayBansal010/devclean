# Scanner architecture

The scanner is organized into four stages:

1. **Definition assembly**: built-in cache definitions, custom definitions, and plugin definitions are merged by canonical name and aliases.
2. **Path resolution**: environment variables are checked first, followed by platform-specific paths and configured cache paths.
3. **Filesystem inspection**: the resolved path is inspected for existence, byte size, file count, directory count, modification time, and file-type totals.
4. **Result filtering**: query, activity, and size constraints are applied while skipped or diagnostic results remain visible to the caller.

This separation keeps discovery policy independent from filesystem measurement and allows commands such as `stats` and `analyze` to reuse scan results consistently.
