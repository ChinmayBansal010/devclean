# Report Schema

All report formats should represent the same normalized analysis model.

## Core fields

- cache identity
- category
- resolved path
- total bytes
- file count
- directory count
- modified time
- age
- active status
- warning state
- growth information when history is available
- recommendation and risk information when analysis is requested

## Stability

Adding a field should not change the meaning of existing fields. Consumers that only need basic scan information should remain compatible with richer analysis output.

JSON is the canonical machine-readable representation. Markdown, CSV, and HTML are presentation formats derived from the same normalized data.
