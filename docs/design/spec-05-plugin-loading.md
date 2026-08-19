# Plugin Loading

Plugins are discovered from configured locations and validated before registration.

A plugin that fails validation is isolated from the remaining registry. Loading must not execute cleanup logic. Duplicate identifiers resolve deterministically and produce a diagnostic so configuration mistakes remain visible.