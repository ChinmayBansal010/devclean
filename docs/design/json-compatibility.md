# JSON Compatibility Policy

JSON output is intended for scripts and other tools, so changes require more discipline than terminal formatting changes.

## Stable fields

Existing field names and meanings should remain stable within a major release. New optional fields are preferred for additive features.

## Null versus missing

A missing field means the producer does not provide the value. A null value means the producer knows the field exists but cannot provide a value. The distinction should be preserved where it is meaningful to consumers.

## Numbers

Byte counts, file counts, and directory counts remain integer values. Ratios and utilization values may use floating-point values with documented semantics.

## Versioning

If a future change requires incompatible JSON semantics, the output format should gain an explicit version rather than relying on undocumented consumer behavior.
