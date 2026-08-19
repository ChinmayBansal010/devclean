# Cancellation Contract

Long-running scans and analyses should observe cancellation between cache units and during bounded filesystem traversal.

Cancellation is cooperative. A cancelled command reports partial completion and does not turn cancellation into a generic failure. Cleanup cancellation follows the same destructive-operation boundary as interruption.