# Cache Registry

The registry is the authoritative collection of cache definitions after built-in, configuration, and plugin sources are merged.

Each definition has a stable identifier. Merge conflicts are deterministic and visible through diagnostics. Registry construction is read-only with respect to the filesystem.