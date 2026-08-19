# Time Thresholds

Age thresholds accept normalized durations and are converted to a single internal unit.

Zero and negative durations are rejected where they would create ambiguous cleanup behavior. Threshold parsing is separate from filesystem timestamp comparison so CLI syntax remains independent from policy logic.