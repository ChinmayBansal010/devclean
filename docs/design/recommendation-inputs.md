# Recommendation Inputs

Recommendations consume normalized scan results rather than raw filesystem state.

Inputs include size, reclaimable size, age, activity, warning state, growth, and policy classification. Recommendations must explain the dominant factors behind each result and distinguish safe actions from suggestions requiring user review.

No recommendation may directly perform cleanup.