# Architecture

`devclean_core` contains the reusable application logic. The scanner layer discovers cache definitions and filesystem data. Command classes translate parsed arguments into operations. Platform helpers isolate filesystem and tool-detection behavior.

Keep destructive operations behind the same path-safety validation used during discovery and configuration loading.
