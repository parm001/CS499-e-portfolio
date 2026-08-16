"""Automated verification for the SQLite security findings tracker."""

from __future__ import annotations

import json
import sqlite3
import tempfile
import unittest
from pathlib import Path

import security_findings_tracker as tracker


class SecurityFindingsTrackerTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_directory.cleanup)
        self.database_path = Path(self.temp_directory.name) / "test_findings.db"
        tracker.initialize_database(self.database_path)
        self.scan_id = tracker.seed_demo_data(self.database_path, reset=True)

    def test_seeded_counts_and_risk_summary(self) -> None:
        overview = tracker.database_overview(self.database_path)
        self.assertEqual(overview["scans"], 1)
        self.assertEqual(overview["findings"], 8)

        risk_counts = {
            row["label"]: row["total"]
            for row in tracker.fetch_grouped_counts(
                self.database_path, "risk_category"
            )
        }
        self.assertEqual(risk_counts, {"NOT RISK": 1, "RISK": 7})

    def test_schema_rejects_invalid_severity(self) -> None:
        invalid = tracker.FindingInput(
            "Invalid severity example",
            "RISK",
            "URGENT",
            "OPEN",
            "MANUAL REVIEW",
            200,
            "This insert should be rejected by the database constraint.",
        )
        with tracker.connect(self.database_path) as connection:
            with self.assertRaises(sqlite3.IntegrityError):
                tracker.insert_finding(connection, self.scan_id, invalid)

    def test_foreign_key_enforcement(self) -> None:
        with tracker.connect(self.database_path) as connection:
            with self.assertRaises(sqlite3.IntegrityError):
                tracker.insert_finding(connection, 999_999, tracker.DEMO_FINDINGS[0])

    def test_parameterized_insert_treats_sql_as_data(self) -> None:
        suspicious_text = "'); DROP TABLE findings; --"
        finding = tracker.FindingInput(
            suspicious_text,
            "NOT RISK",
            "LOW",
            "OPEN",
            "MANUAL REVIEW",
            999,
            "Verify that bound parameters store this text without executing it.",
        )
        with tracker.connect(self.database_path) as connection:
            tracker.insert_finding(connection, self.scan_id, finding)
            stored = connection.execute(
                "SELECT issue_name FROM findings WHERE line_number = ?", (999,)
            ).fetchone()
            table_exists = connection.execute(
                """
                SELECT COUNT(*) AS total
                FROM sqlite_master
                WHERE type = 'table' AND name = 'findings'
                """
            ).fetchone()
        self.assertEqual(stored["issue_name"], suspicious_text)
        self.assertEqual(table_exists["total"], 1)

    def test_status_update_creates_history_record(self) -> None:
        updated = tracker.update_status(self.database_path, 1, "OPEN")
        self.assertTrue(updated)
        history = tracker.fetch_status_history(self.database_path)
        self.assertEqual(len(history), 1)
        self.assertEqual(history[0]["old_status"], "REMEDIATED")
        self.assertEqual(history[0]["new_status"], "OPEN")

    def test_priority_report_and_json_export(self) -> None:
        priority = tracker.list_priority_findings(self.database_path)
        self.assertEqual(len(priority), 7)
        self.assertTrue(all(row["severity"] in {"CRITICAL", "HIGH"} for row in priority))

        output_path = Path(self.temp_directory.name) / "findings.json"
        tracker.export_to_json(self.database_path, output_path)
        payload = json.loads(output_path.read_text(encoding="utf-8"))
        self.assertEqual(len(payload["scans"]), 1)
        self.assertEqual(len(payload["findings"]), 8)
        self.assertIn("severity", payload["summaries"])


if __name__ == "__main__":
    unittest.main(verbosity=2)
