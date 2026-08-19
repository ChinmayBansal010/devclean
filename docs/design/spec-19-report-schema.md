# Report Schema

Reports use a normalized cache record containing identity, category, path, size, activity, safety, and diagnostic fields.

Renderers may omit presentation-only fields, but they must not reinterpret core measurements. Stable identifiers allow future report formats to consume the same model.