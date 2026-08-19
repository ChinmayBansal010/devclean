# Cleanup Explanations

Every cleanup decision should have a machine-readable reason.

Reasons include `safe_candidate`, `active_cache`, `protected_path`, `stale_candidate`, `target_reached`, and `policy_rejected`.

Human output may turn these into concise sentences, while JSON should preserve the stable reason code and relevant measurements.

This keeps diagnostics useful without coupling the planner to presentation code.