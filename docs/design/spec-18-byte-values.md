# Byte Value Parsing

Human-readable byte targets use decimal or binary suffixes consistently according to the documented CLI grammar.

Parsing rejects overflow, unknown units, and trailing garbage. Internal calculations use bytes and report both requested and achieved values so a target such as `10GB` is never silently reinterpreted.