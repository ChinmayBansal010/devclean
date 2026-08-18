# Roadmap

## v1.x

### Storage intelligence

- Harden incremental scanning and scan caching
- Expand doctor-style health diagnostics
- Improve historical growth analysis and storage-budget recommendations
- Add broader duplicate-candidate analysis

### Cleanup safety

- Strengthen deterministic cleanup planning
- Expand stale-file policy controls
- Improve cleanup manifest auditing and recovery workflows
- Add broader integration coverage for protected and platform-specific paths

### Developer experience

- Improve package metadata for downstream package managers
- Expand report contracts and machine-readable output
- Improve terminal dashboard workflows

## v2.0 ideas

- Richer scheduler support
- Native structured logging configuration
- More detailed profiling output
- Additional plugin packaging workflows
- Optional restore workflows backed by cleanup manifests

New work should preserve cross-platform behavior, conservative filesystem safety, deterministic planning, and compatibility with existing plugin and report contracts.
