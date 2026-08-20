# Permission Failures

Permission errors are expected on some filesystem trees and should be represented explicitly in scan results.

A permission failure should not be reported as an empty directory or as a successful cleanup. The command should preserve useful progress and return an appropriate status when the failure affects the requested operation.
