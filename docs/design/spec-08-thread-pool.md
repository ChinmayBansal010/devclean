# Thread Pool Boundary

Worker threads are limited to CPU and filesystem measurement tasks. Command dispatch, terminal rendering, and destructive policy decisions remain on the coordinating path.

Workers return values rather than mutating shared presentation state. Shutdown must join all workers before command completion.