# Release Gates

A release candidate must configure cleanly, compile with supported toolchains, pass the complete test suite, and produce expected package metadata.

Release checks also verify version consistency and generated artifacts. A failing optional lint tool should be reported separately from a failing build or test gate.