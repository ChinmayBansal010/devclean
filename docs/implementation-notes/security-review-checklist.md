# Security Review Checklist

Before a release, review path handling, symlink behavior, plugin loading, configuration parsing, report rendering, and destructive-operation confirmation.

Security review should focus on boundaries where user-controlled paths or metadata can influence filesystem operations or generated output.
