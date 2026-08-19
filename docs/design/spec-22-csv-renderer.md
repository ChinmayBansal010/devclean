# CSV Renderer

CSV output uses a fixed column order and escaped fields according to standard CSV rules.

Rows are sorted deterministically by cache identifier. Missing values remain empty rather than being replaced with formatted placeholders. The renderer does not perform filtering or cleanup decisions.