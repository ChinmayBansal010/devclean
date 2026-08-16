# Filter matrix

The command layer should preserve the same selection semantics across commands.

| Command | active-only | min-size | max-size | name filters |
| --- | --- | --- | --- | --- |
| scan | yes | yes | yes | yes |
| stats | yes | yes | yes | yes |
| analyze | yes | yes | yes | yes |
| clean | yes | yes | yes | yes |

Every combination should be exercised by either a parser test, integration test, or CLI smoke test before a release.
