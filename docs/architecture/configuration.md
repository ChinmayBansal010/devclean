# Configuration architecture

Configuration is loaded into `AppConfig` and then consumed by command and scanner layers.

The configuration model contains disabled caches, ignored caches, default presentation settings, schema version, application version, and custom cache definitions.

Migration should be explicit: when an older schema is encountered, the loader converts known fields into the current model before saving. Unknown data should not be interpreted as executable configuration.

Custom cache definitions are merged with built-in definitions by canonical name and aliases. A custom definition must not silently replace a built-in cache with a different location unless the user explicitly configured it.
