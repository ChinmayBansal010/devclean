# Interrupted Cleanup

SIGINT or equivalent interruption stops scheduling new destructive operations and lets the current operation reach a safe boundary.

The final report identifies that execution was interrupted and distinguishes completed work from remaining planned work. No automatic second pass is started.