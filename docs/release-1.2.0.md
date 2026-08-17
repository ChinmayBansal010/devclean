# devclean 1.2.0 Release Notes

## Highlights

Version 1.2.0 consolidates the cache-analysis work introduced after the 1.0.x line into a release focused on safer decisions, storage intelligence, and reusable analysis infrastructure.

### User-facing capabilities already available

- `doctor` developer-environment diagnostics.
- Target-based cleanup with `--target`.
- Stale-file cleanup with `--stale`.
- Safe cleanup with `--safe`.
- `recommend` health scoring and ranked cleanup guidance.
- `dashboard` interactive cache-health view.
- Historical growth analysis.
- Structured JSON, CSV, HTML, and Markdown reporting.

### Core engineering additions

The release also establishes reusable components for:

- storage budget calculations;
- deterministic cleanup planning;
- cache policy evaluation;
- historical scan comparison;
- centralized path safety inspection;
- stale-file age indexing;
- resilient directory size estimation;
- cleanup operation manifests;
- normalized report generation;
- developer-environment summaries;
- duplicate-file candidate grouping; and
- cache growth projection.

These components are intentionally separated from command implementations so future commands can reuse the same policy and analysis primitives instead of duplicating safety logic.

## Compatibility

The release preserves the existing command model and keeps the existing cleanup flags backward compatible. No major command syntax removal is introduced by this release.

## Verification checklist

Before publishing the release artifact:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/devclean --version
./build/devclean scan --json
./build/devclean recommend --json
./build/devclean clean --safe --target 1GB --dry-run
```

The release should not be tagged until the local build and the GitHub Actions matrix both pass.