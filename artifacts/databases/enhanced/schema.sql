PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS scans (
    id INTEGER PRIMARY KEY,
    artifact_name TEXT NOT NULL
        CHECK (length(trim(artifact_name)) > 0),
    scanner TEXT NOT NULL
        CHECK (length(trim(scanner)) > 0),
    scanned_at TEXT NOT NULL,
    notes TEXT NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS findings (
    id INTEGER PRIMARY KEY,
    scan_id INTEGER NOT NULL,
    issue_name TEXT NOT NULL
        CHECK (length(trim(issue_name)) > 0),
    risk_category TEXT NOT NULL
        CHECK (risk_category IN ('RISK', 'NOT RISK')),
    severity TEXT NOT NULL
        CHECK (severity IN ('LOW', 'MEDIUM', 'HIGH', 'CRITICAL')),
    status TEXT NOT NULL DEFAULT 'OPEN'
        CHECK (status IN ('OPEN', 'IN PROGRESS', 'REMEDIATED', 'ACCEPTED')),
    source_tool TEXT NOT NULL
        CHECK (source_tool IN ('VISUAL STUDIO', 'CPPCHECK', 'MANUAL REVIEW')),
    line_number INTEGER
        CHECK (line_number IS NULL OR line_number > 0),
    remediation TEXT NOT NULL
        CHECK (length(trim(remediation)) > 0),
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (scan_id) REFERENCES scans(id) ON DELETE CASCADE,
    UNIQUE (scan_id, issue_name, source_tool, line_number)
);

CREATE TABLE IF NOT EXISTS finding_status_history (
    id INTEGER PRIMARY KEY,
    finding_id INTEGER NOT NULL,
    old_status TEXT NOT NULL
        CHECK (old_status IN ('OPEN', 'IN PROGRESS', 'REMEDIATED', 'ACCEPTED')),
    new_status TEXT NOT NULL
        CHECK (new_status IN ('OPEN', 'IN PROGRESS', 'REMEDIATED', 'ACCEPTED')),
    changed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (finding_id) REFERENCES findings(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_findings_scan
    ON findings (scan_id);

CREATE INDEX IF NOT EXISTS idx_findings_risk_severity
    ON findings (risk_category, severity);

CREATE INDEX IF NOT EXISTS idx_findings_status
    ON findings (status);

CREATE INDEX IF NOT EXISTS idx_status_history_finding
    ON finding_status_history (finding_id, changed_at);

CREATE TRIGGER IF NOT EXISTS log_finding_status_change
AFTER UPDATE OF status ON findings
FOR EACH ROW
WHEN OLD.status <> NEW.status
BEGIN
    INSERT INTO finding_status_history (finding_id, old_status, new_status)
    VALUES (NEW.id, OLD.status, NEW.status);
END;
