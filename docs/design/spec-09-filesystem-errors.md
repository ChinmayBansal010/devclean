# Filesystem Error Handling

Filesystem failures are classified as missing, inaccessible, transient, or unexpected.

A missing cache is a normal scan condition. Permission failures become warnings unless the command explicitly requires access. Unexpected failures are retained with context and must not cause cleanup of a partially understood path.