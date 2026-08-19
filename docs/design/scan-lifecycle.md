# Scan Lifecycle

A scan progresses through discovery, normalization, filtering, measurement, enrichment, and result publication.

Discovery finds configured and detected cache sources. Normalization resolves paths and metadata. Filtering applies ignore and platform policies. Measurement calculates filesystem statistics. Enrichment adds activity, growth, and safety information.

Each stage should be independently testable and should avoid mutating the filesystem.