# Platform Policy

Platform-specific cache rules are isolated from generic scanning logic.

A platform policy defines supported paths, path separators, executable discovery, and safety exceptions. Unsupported platforms should degrade to generic rules rather than pretending native semantics are available.