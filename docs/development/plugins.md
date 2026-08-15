# Plugins

Plugins are JSON cache definitions loaded from the user's devclean plugin directory. A plugin should provide a valid name, a safe absolute path for the current platform, and optional aliases, environment variables, OS support, and priority metadata.

Invalid names and unsafe paths are rejected before a plugin is registered.
