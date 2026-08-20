# Path Normalization

Filesystem paths should be normalized before they are compared, classified, or placed in a cleanup manifest.

Normalization should preserve the distinction between an existing path and a missing path, avoid accidental traversal outside the requested scope, and use platform-native path semantics.
