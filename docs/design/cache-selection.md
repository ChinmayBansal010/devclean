# Cache Selection Model

Cleanup selection is deterministic and explainable.

Candidates are ranked by safety first, then reclaimable bytes, current size, and stable name ordering. Active caches and explicitly protected locations are never promoted above safe candidates. The planner should return both the selected candidates and the reason each candidate was accepted or rejected.

Selection must remain independent of terminal formatting so JSON, text, and future TUI renderers receive the same decisions.

A target budget is treated as an upper bound, not a promise that the exact number of bytes can be recovered.