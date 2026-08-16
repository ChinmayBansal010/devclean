# Release procedure

1. Start from a clean `main` checkout.
2. Update the version in the application configuration and packaging metadata.
3. Configure with the vcpkg toolchain.
4. Build Debug and Release configurations.
5. Run CTest with failure output enabled.
6. Run CLI smoke checks for help, version, scan, stats, analyze, and dry-run cleanup.
7. Run the sanitizer build on supported GCC/Clang environments.
8. Build the package artifacts.
9. Review `git diff` and generated package contents.
10. Tag the release only after all checks succeed.
