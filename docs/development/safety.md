# Cleanup safety

Cleanup must never operate on protected filesystem locations. Custom cache definitions require absolute paths and are rejected when they resolve through protected locations or contain parent traversal components.

Use `clean --dry-run` before destructive cleanup when inspecting a new cache definition.
