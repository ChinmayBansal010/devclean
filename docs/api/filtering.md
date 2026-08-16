# Filtering contract

The scanner accepts name and alias filters plus size and activity constraints.

## Name filters

Filters are matched case-insensitively against the canonical cache name and aliases.

## Size filters

`--min-size` excludes entries below the threshold.
`--max-size` excludes entries above the threshold.

Both limits are inclusive. When both are present, the minimum must not exceed the maximum.

## Activity filters

`--active-only` retains caches associated with an installed tool.

The same filter semantics should be preserved by `scan`, `stats`, `analyze`, and `clean` so that users can safely move from inspection to cleanup without changing the selected dataset.
