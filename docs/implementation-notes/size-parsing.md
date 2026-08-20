# Size Parsing

Size arguments should be parsed once into an unsigned byte representation and validated for overflow before they reach scanning or cleanup logic.

Accepted units should be documented consistently across commands. Invalid or overflowing values should fail safely rather than wrapping into a smaller size.
