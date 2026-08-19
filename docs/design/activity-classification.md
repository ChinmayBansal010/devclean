# Activity Classification

Cache activity combines filesystem timestamps, detected tool state, and stored scan history.

The classification vocabulary is `active`, `recent`, `stale`, `dormant`, and `unknown`. Unknown is preferred over guessing when evidence is incomplete. Activity affects recommendation priority but never overrides explicit safety policy.

Classification inputs remain separate from presentation so thresholds can evolve without changing scan result structure.