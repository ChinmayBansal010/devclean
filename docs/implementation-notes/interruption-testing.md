# Interruption Testing

Cancellation behavior should be tested during discovery, planning, and cleanup rather than only at command startup.

An interrupted operation should leave the filesystem in a state consistent with the work already completed and should provide a clear indication that the operation did not finish normally.
