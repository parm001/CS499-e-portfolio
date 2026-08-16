"""SQLite-backed tracker for security findings from QuestionableCode.cpp.

The program demonstrates schema creation, constraints, foreign keys, indexes,
parameterized data operations, audit history, aggregate reporting, and JSON
export using only Python's standard library.
"""

from __future__ import annotations

import argparse
import json
import sqlite3
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence


DEFAULT_DB_PATH = Path(__file__).with_name("security_findings.db")
SCHEMA_PATH = Path(__file__).with_name("schema.sql")

ALLOWED_RISK_CATEGORIES = ("RISK", "NOT RISK")
ALLOWED_SEVERITIES = ("LOW", "MEDIUM", "HIGH", "CRITICAL")
ALLOWED_STATUSES = ("OPEN", "IN PROGRESS", "REMEDIATED", "ACCEPTED")
ALLOWED_TOOLS = ("VISUAL STUDIO", "CPPCHECK", "MANUAL REVIEW")


@dataclass(frozen=True)
class FindingInput:
    issue_name: str
    risk_category: str
    severity: str
    status: str
    source_tool: str
    line_number: int | None
    remediation: str


DEMO_FINDINGS: tuple[FindingInput, ...] = (
    FindingInput(
        "Endless recursion",
        "RISK",
        "CRITICAL",
        "REMEDIATED",
        "CPPCHECK",
        36,
        "Replaced the recursive call with a direct set membership lookup.",
    ),
    FindingInput(
        "Throw from noexcept function",
        "RISK",
        "HIGH",
        "REMEDIATED",
        "CPPCHECK",
        50,
        "Replaced the exception path with a result value that honors noexcept.",
    ),
    FindingInput(
        "Pointer to local variable",
        "RISK",
        "CRITICAL",
        "REMEDIATED",
        "MANUAL REVIEW",
        57,
        "Returned an optional value instead of exposing a dangling pointer.",
    ),
    FindingInput(
        "Out-of-bounds array write",
        "RISK",
        "CRITICAL",
        "REMEDIATED",
        "CPPCHECK",
        64,
        "Replaced unchecked raw-array access with a bounds-checked vector update.",
    ),
    FindingInput(
        "Iterator invalidation",
        "RISK",
        "HIGH",
        "REMEDIATED",
        "CPPCHECK",
        88,
        "Used the erase-remove idiom instead of erasing through an invalidated iterator.",
    ),
    FindingInput(
        "Null pointer loop bug",
        "RISK",
        "CRITICAL",
        "REMEDIATED",
        "CPPCHECK",
        107,
        "Removed the stray semicolon and added explicit null-checked traversal.",
    ),
    FindingInput(
        "Assignment inside assert",
        "RISK",
        "HIGH",
        "REMEDIATED",
        "VISUAL STUDIO",
        127,
        "Separated assignment from validation and used an equality comparison.",
    ),
    FindingInput(
        "Variable shadowing",
        "NOT RISK",
        "LOW",
        "ACCEPTED",
        "CPPCHECK",
        134,
        "Documented as a maintainability issue and reduced shadowing in the enhanced code.",
    ),
)


def connect(database_path: Path = DEFAULT_DB_PATH) -> sqlite3.Connection:
    """Open a configured SQLite connection with referential integrity enabled."""
    connection = sqlite3.connect(database_path)
    connection.row_factory = sqlite3.Row
    connection.execute("PRAGMA foreign_keys = ON")
    return connection


def initialize_database(database_path: Path = DEFAULT_DB_PATH) -> None:
    """Create all tables, constraints, indexes, and triggers from schema.sql."""
    schema = SCHEMA_PATH.read_text(encoding="utf-8")
    with connect(database_path) as connection:
        connection.executescript(schema)


def create_scan(
    connection: sqlite3.Connection,
    artifact_name: str,
    scanner: str,
    notes: str = "",
) -> int:
    """Create one scan and return its primary key."""
    scanned_at = datetime.now(timezone.utc).isoformat(timespec="seconds")
    cursor = connection.execute(
        """
        INSERT INTO scans (artifact_name, scanner, scanned_at, notes)
        VALUES (?, ?, ?, ?)
        """,
        (artifact_name, scanner, scanned_at, notes),
    )
    return int(cursor.lastrowid)


def insert_finding(
    connection: sqlite3.Connection,
    scan_id: int,
    finding: FindingInput,
) -> int:
    """Insert one validated finding using bound parameters."""
    cursor = connection.execute(
        """
        INSERT INTO findings (
            scan_id,
            issue_name,
            risk_category,
            severity,
            status,
            source_tool,
            line_number,
            remediation
        )
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """,
        (
            scan_id,
            finding.issue_name,
            finding.risk_category,
            finding.severity,
            finding.status,
            finding.source_tool,
            finding.line_number,
            finding.remediation,
        ),
    )
    return int(cursor.lastrowid)


def seed_demo_data(database_path: Path = DEFAULT_DB_PATH, reset: bool = False) -> int:
    """Load representative findings and return the new scan id."""
    initialize_database(database_path)
    with connect(database_path) as connection:
        if reset:
            connection.execute("DELETE FROM scans")
        scan_id = create_scan(
            connection,
            "QuestionableCode.cpp",
            "Visual Studio, Cppcheck, and manual review",
            "CS 499 database enhancement demonstration data.",
        )
        for finding in DEMO_FINDINGS:
            insert_finding(connection, scan_id, finding)
    return scan_id


def update_status(
    database_path: Path,
    finding_id: int,
    new_status: str,
) -> bool:
    """Update a finding status and let the schema trigger record the change."""
    if new_status not in ALLOWED_STATUSES:
        raise ValueError(f"Unsupported status: {new_status}")
    with connect(database_path) as connection:
        cursor = connection.execute(
            """
            UPDATE findings
            SET status = ?, updated_at = CURRENT_TIMESTAMP
            WHERE id = ?
            """,
            (new_status, finding_id),
        )
        return cursor.rowcount == 1


def fetch_grouped_counts(
    database_path: Path,
    field: str,
) -> list[sqlite3.Row]:
    """Return grouped counts for one allow-listed reporting dimension."""
    allowed_fields = {
        "risk_category": "risk_category",
        "severity": "severity",
        "status": "status",
        "source_tool": "source_tool",
    }
    column = allowed_fields.get(field)
    if column is None:
        raise ValueError(f"Unsupported report field: {field}")

    # The column name comes only from the fixed allow-list above. Values remain bound.
    query = f"""
        SELECT {column} AS label, COUNT(*) AS total
        FROM findings
        GROUP BY {column}
        ORDER BY total DESC, label ASC
    """
    with connect(database_path) as connection:
        return list(connection.execute(query).fetchall())


def list_priority_findings(database_path: Path) -> list[sqlite3.Row]:
    """Return high-priority risk findings for review."""
    with connect(database_path) as connection:
        return list(
            connection.execute(
                """
                SELECT
                    f.id,
                    f.issue_name,
                    f.severity,
                    f.status,
                    f.source_tool,
                    f.line_number,
                    f.remediation
                FROM findings AS f
                WHERE f.risk_category = ?
                  AND f.severity IN (?, ?)
                ORDER BY
                    CASE f.severity WHEN 'CRITICAL' THEN 1 ELSE 2 END,
                    f.issue_name ASC
                """,
                ("RISK", "CRITICAL", "HIGH"),
            ).fetchall()
        )


def fetch_status_history(database_path: Path) -> list[sqlite3.Row]:
    """Return status transitions with their related issue names."""
    with connect(database_path) as connection:
        return list(
            connection.execute(
                """
                SELECT
                    h.id,
                    h.finding_id,
                    f.issue_name,
                    h.old_status,
                    h.new_status,
                    h.changed_at
                FROM finding_status_history AS h
                JOIN findings AS f ON f.id = h.finding_id
                ORDER BY h.id ASC
                """
            ).fetchall()
        )


def database_overview(database_path: Path) -> sqlite3.Row:
    """Return top-level record totals."""
    with connect(database_path) as connection:
        return connection.execute(
            """
            SELECT
                (SELECT COUNT(*) FROM scans) AS scans,
                (SELECT COUNT(*) FROM findings) AS findings,
                (SELECT COUNT(*) FROM finding_status_history) AS status_changes
            """
        ).fetchone()


def format_report(database_path: Path) -> str:
    """Build a repeatable, audience-friendly command-line report."""
    overview = database_overview(database_path)
    sections: list[str] = [
        "SQLite Security Findings Tracker",
        "=" * 34,
        f"Database: {database_path}",
        f"Scans: {overview['scans']}",
        f"Findings: {overview['findings']}",
        f"Recorded status changes: {overview['status_changes']}",
    ]

    labels = (
        ("Risk classification", "risk_category"),
        ("Severity", "severity"),
        ("Status", "status"),
        ("Source tool", "source_tool"),
    )
    for title, field in labels:
        sections.extend(("", f"{title} summary"))
        for row in fetch_grouped_counts(database_path, field):
            sections.append(f"- {row['label']}: {row['total']}")

    sections.extend(("", "High-priority findings"))
    for row in list_priority_findings(database_path):
        location = f"line {row['line_number']}" if row["line_number"] else "line unknown"
        sections.append(
            f"- [{row['severity']}/{row['status']}] {row['issue_name']} "
            f"({row['source_tool']}, {location})"
        )

    return "\n".join(sections)


def export_to_json(database_path: Path, output_path: Path) -> None:
    """Export scans, findings, summaries, and history to a portable JSON file."""
    with connect(database_path) as connection:
        scans = [dict(row) for row in connection.execute("SELECT * FROM scans ORDER BY id")]
        findings = [
            dict(row)
            for row in connection.execute(
                "SELECT * FROM findings ORDER BY scan_id, id"
            )
        ]

    payload = {
        "generated_at": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "database": str(database_path),
        "scans": scans,
        "findings": findings,
        "summaries": {
            field: [dict(row) for row in fetch_grouped_counts(database_path, field)]
            for field in ("risk_category", "severity", "status", "source_tool")
        },
        "status_history": [dict(row) for row in fetch_status_history(database_path)],
    }
    output_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Track and report security findings with SQLite."
    )
    parser.add_argument(
        "--db",
        type=Path,
        default=DEFAULT_DB_PATH,
        help="SQLite database path (default: enhanced/security_findings.db)",
    )
    subparsers = parser.add_subparsers(dest="command")

    subparsers.add_parser("init", help="Create the database schema.")

    seed_parser = subparsers.add_parser("seed", help="Load representative findings.")
    seed_parser.add_argument(
        "--reset",
        action="store_true",
        help="Delete existing scans before loading the demonstration data.",
    )

    subparsers.add_parser("report", help="Print summary and priority reports.")

    update_parser = subparsers.add_parser("update-status", help="Change one finding status.")
    update_parser.add_argument("finding_id", type=int)
    update_parser.add_argument("status", choices=ALLOWED_STATUSES)

    export_parser = subparsers.add_parser("export", help="Export all records to JSON.")
    export_parser.add_argument("output", type=Path)

    subparsers.add_parser("demo", help="Reset, seed, and report in one command.")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    command = args.command or "demo"
    database_path = args.db.expanduser().resolve()

    try:
        if command == "init":
            initialize_database(database_path)
            print(f"Initialized database: {database_path}")
        elif command == "seed":
            scan_id = seed_demo_data(database_path, reset=args.reset)
            print(f"Loaded demonstration scan {scan_id} into {database_path}")
        elif command == "report":
            initialize_database(database_path)
            print(format_report(database_path))
        elif command == "update-status":
            initialize_database(database_path)
            updated = update_status(database_path, args.finding_id, args.status)
            if not updated:
                print(f"Finding {args.finding_id} was not found.", file=sys.stderr)
                return 1
            print(f"Finding {args.finding_id} updated to {args.status}.")
        elif command == "export":
            initialize_database(database_path)
            export_to_json(database_path, args.output.expanduser().resolve())
            print(f"Exported report: {args.output.expanduser().resolve()}")
        elif command == "demo":
            seed_demo_data(database_path, reset=True)
            print(format_report(database_path))
        else:
            raise ValueError(f"Unsupported command: {command}")
    except (OSError, sqlite3.Error, ValueError) as error:
        print(f"Database operation failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
