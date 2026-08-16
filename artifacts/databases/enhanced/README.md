# SQLite Security Findings Tracker

## Purpose

This database enhancement converts findings from the original `QuestionableCode.cpp` static-analysis exercise into persistent, validated, and reportable data. It demonstrates database design and secure data handling while preserving the security-review purpose of the original artifact.

## Technical Files

- `security_findings_tracker.py`: command-line application and database access layer.
- `schema.sql`: normalized tables, primary and foreign keys, constraints, indexes, and audit trigger.
- `test_security_findings_tracker.py`: automated database and security tests.
- `sample_output.txt`: expected output from the demonstration report.
- `test_results.txt`: recorded automated test results.

The runtime database file is intentionally excluded from the submission. The program recreates it from `schema.sql`, which keeps the artifact portable and avoids submitting generated state.

## Database Design

The schema uses three related tables:

- `scans` stores the reviewed artifact, scanner information, time, and notes.
- `findings` stores each issue, risk classification, severity, status, source tool, line number, and remediation. Each finding belongs to a scan through a foreign key.
- `finding_status_history` stores status changes recorded automatically by a trigger.

Database protections include required fields, `CHECK` constraints for allowed classifications, severities, statuses, and tools, a positive line-number constraint, a uniqueness rule, foreign-key enforcement, cascading cleanup, and indexes supporting common reports.

All user-controlled values are sent to SQLite with parameterized statements. The only dynamic report column is selected from a fixed allow-list before the SQL statement is created.

## Run the Demonstration

From the `enhanced` directory:

```bash
python3 security_findings_tracker.py demo
```

The default behavior with no subcommand also runs the demonstration:

```bash
python3 security_findings_tracker.py
```

Use another database path when desired:

```bash
python3 security_findings_tracker.py --db my_findings.db demo
```

## Other Commands

```bash
python3 security_findings_tracker.py init
python3 security_findings_tracker.py seed --reset
python3 security_findings_tracker.py report
python3 security_findings_tracker.py update-status 1 OPEN
python3 security_findings_tracker.py export findings_report.json
```

## Run the Tests

```bash
python3 -m unittest -v test_security_findings_tracker.py
```

The tests verify representative record counts, risk summaries, database constraints, foreign-key enforcement, SQL-injection resistance through parameter binding, automatic status-history recording, priority queries, and JSON export.

## Skills Demonstrated

This enhancement demonstrates relational modeling, normalization, schema constraints, primary and foreign keys, indexing, transactions, parameterized CRUD operations, joins, aggregate queries, audit history, reporting, export, automated verification, portability, and security-minded database development.
