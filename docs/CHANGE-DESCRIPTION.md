# Proposed change description

## Staff screen viewer

**Problem:** When a patron needs help, staff cannot see the relevant workstation screen from the Libki administration page.

**Change:** Add an opt-in **View screen** action for one configured client. The staff member selects a reason, Libki queues a notice and records the request, and a short-lived token connects an embedded noVNC viewer through a fixed-target gateway. The workstation's VNC service is configured to require local approval and to prevent remote input and file transfers.

**Example:** A patron cannot locate a download. Staff choose **Patron requested assistance**, the patron accepts the workstation prompt, and staff can explain the next step while the patron retains control.

The optional client patch adds a visible Windows connection indicator. It is independent of the server integration and can be reviewed separately.

## Public adaptation

The export uses public upstream baselines and contains no private commit history. Deployment names and addresses are examples. Compared with the original implementation, this package also:

- installs the complete pinned noVNC dependency set, including pako;
- rejects cross-origin requests, user/request instance mismatches, and requests outside the configured viewer instance;
- cancels startup when a modal closes or a different workstation is selected;
- supports dotted workstation names in signed tokens;
- makes the client indicator opt-in and describes what it actually detects;
- keeps gateway nonce state across ordinary container restarts using a named volume.

## Review scope

The initial server pull request would include the server patch, gateway, dependency installer, and relevant documentation/tests. The client pull request would include only the indicator and build integration.

Do not describe the request audit as proof that a session was viewed: it records a request, not endpoint acceptance, completed viewing, or a disconnect. Automated results and remaining workstation tests are listed in [VALIDATION.md](VALIDATION.md).
