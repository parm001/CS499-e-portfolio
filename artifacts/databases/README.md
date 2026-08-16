# CS 499 Milestone Four: Enhancement Three—Databases

This submission contains the original artifact and the enhanced database artifact for Parminder Singh's CS 499 ePortfolio.

## Contents

- `original/QuestionableCode.cpp`: unchanged source artifact from the CS 405/CS 410 static-analysis exercise.
- `original/README.md`: context for the intentionally unsafe original file.
- `enhanced/schema.sql`: relational database definition with constraints, keys, indexes, and an audit trigger.
- `enhanced/security_findings_tracker.py`: SQLite-backed command-line application.
- `enhanced/test_security_findings_tracker.py`: automated verification suite.
- `enhanced/sample_output.txt`: representative database report.
- `enhanced/test_results.txt`: recorded result of the six automated tests.
- `enhanced/README.md`: design explanation and usage instructions.

The runtime SQLite database is generated from `schema.sql` and is not included. From the `enhanced` directory, run `python3 security_findings_tracker.py demo` to create a database, load the demonstration findings, and print the report. Run `python3 -m unittest -v test_security_findings_tracker.py` to execute the verification suite.

The original C++ source contains deliberate security and correctness defects for analysis purposes. It should not be used as production code.
