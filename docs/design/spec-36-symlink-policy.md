# Symlink Policy

Symlinks require explicit handling because their target may escape a cache boundary.

Cleanup must inspect link identity before mutation and must not recursively follow a symlink as if it were an ordinary directory. Broken links remain reportable without being treated as evidence that a target is safe to remove.