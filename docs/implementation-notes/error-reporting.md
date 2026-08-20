# Error Reporting

Errors should retain enough context to explain which operation failed without exposing unnecessary filesystem details.

A useful diagnostic identifies the command, operation, affected path or category when safe, and the recovery or next action. Errors that affect only one candidate should not hide successful processing of unrelated candidates.
