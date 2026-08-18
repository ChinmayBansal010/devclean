# Plugin Lifecycle

Plugins extend cache discovery without changing the core registry for every new tool.

## Lifecycle

A plugin is discovered, loaded, validated, and then asked for cache definitions. Invalid plugin metadata must be isolated from valid built-in definitions.

## Compatibility

The plugin contract should expose an explicit version so incompatible plugins can be rejected before invocation. Plugin failures must not make unrelated cache definitions disappear.

## Isolation

Plugins provide metadata and discovery information. They must not bypass the core cleanup safety layer. Filesystem mutation remains the responsibility of the core cleanup engine.

## Diagnostics

Plugin load failures should be visible in scan warnings and doctor output. Silent failure makes cache coverage difficult to understand and debug.

## Future evolution

A capability field can allow plugins to advertise optional features such as stale-file support or active-process detection without forcing every plugin to implement them.
