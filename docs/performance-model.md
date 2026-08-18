# Performance Model

`devclean` performs filesystem work that can become expensive on large developer machines. Performance decisions should therefore be explicit.

## Scan cost

The dominant costs are directory traversal, metadata inspection, and optional history analysis. Cache definitions that are disabled or filtered out should not trigger filesystem work.

## Parallelism

Independent cache roots may be scanned concurrently. Shared mutable state should remain outside worker callbacks or use explicit synchronization.

## Size estimation

Directory-size estimation should tolerate permission failures and continue where possible. The result should identify incomplete measurements rather than silently treating an inaccessible subtree as empty.

## Reporting

Analysis should operate on normalized in-memory results so JSON, CSV, Markdown, and HTML formatting do not repeat filesystem traversal.
