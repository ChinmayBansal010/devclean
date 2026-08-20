# Configuration Migration Testing

Configuration migration tests should cover supported older formats, missing optional fields, invalid values, and interrupted writes.

A successful migration should preserve user intent and produce a configuration that can be loaded by the current version without warnings that hide real compatibility problems.
