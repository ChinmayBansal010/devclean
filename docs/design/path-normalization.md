# Path Normalization

Filesystem paths are normalized before comparison, filtering, reporting, or cleanup.

Normalization must preserve platform semantics while removing redundant separators and equivalent relative segments. Home-directory expansion occurs before policy checks. Canonicalization that requires filesystem access is used only when required because missing paths are valid scan results.

Comparisons must use normalized forms so duplicate definitions cannot create duplicate cleanup work.