# Command Dispatch Contract

Command dispatch maps parsed command names to isolated command handlers. Parsing errors are distinct from execution errors. Dispatch must not perform filesystem work before a validated command reaches its handler.

This boundary keeps CLI syntax independent from core cleanup logic.