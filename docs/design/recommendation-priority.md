# Recommendation Priority

Recommendation priority is ordered as critical, high, moderate, low, and informational.

Priority considers reclaimable space and observed pressure while respecting activity and safety state. A large active cache can remain informational while a smaller stale cache becomes high priority.

The priority label is advisory and never changes the destructive-operation policy.