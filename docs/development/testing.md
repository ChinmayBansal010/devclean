# Testing

Build the project before running CTest. Use `ctest --test-dir build --output-on-failure` for the complete suite.

Unit tests should validate individual parsing and formatting behavior. Integration tests should use temporary filesystem fixtures and avoid modifying the user's real caches.
