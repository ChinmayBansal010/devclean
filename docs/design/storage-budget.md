# Storage Budget Design

## Purpose

The storage budget layer converts a cleanup request into a deterministic storage objective. It separates the question of how much space should be recovered from the filesystem mutation that performs cleanup.

## Inputs

- Requested byte limit
- Current cache footprint
- Reclaimable bytes
- Candidate cache metadata
- Safety classification

## Invariants

Reclaimable bytes must never exceed the current footprint. A zero limit represents an unbounded budget rather than an invalid negative target. Candidate ordering must remain deterministic so repeated dry runs produce stable plans.

## Selection policy

Safe candidates are preferred before unsafe candidates. Within the same safety class, larger reclaimable candidates are considered first. Name ordering is used as the final tie breaker.

## Safety boundary

The budget planner does not delete files. It produces planning data that a cleanup engine can validate again immediately before mutation. This keeps arithmetic and filesystem effects independently testable.

## Future extension

The model can later incorporate age, activity, growth rate, and cache-specific risk without changing the cleanup engine contract.
