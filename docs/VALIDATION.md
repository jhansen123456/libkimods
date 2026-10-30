# Validation record

Validation date: 2026-10-30. Results refer to the exact upstream baselines in [INSTALL.md](INSTALL.md) plus these patches.

## Completed automated checks

- Server: 34 assertions covering default-off configuration, signatures, dotted names, unsupported clients/reasons, administrator access, user/request instance agreement and configured-instance isolation, cross-origin/header rejection, method restriction, notice/audit failures, successful issue, and no-cache headers. Controller tests use synthetic context/database fixtures.
- Gateway: five test cases covering dotted names, malformed/expired/future/tampered tokens, unknown clients, concurrent replay across worker processes, unavailable nonce storage, and invalid target addresses.
- Gateway runtime: a real websockify process connected over loopback to a synthetic RFB endpoint; valid handshake and RFB banner succeeded, while replay, a wrong client, and tampering were rejected. Captured logs contained no token URLs.
- Browser: Chromium loaded the complete pinned noVNC module; the actual patched UI, exercised with synthetic API/RFB fixtures, set view-only mode, disconnected on close, and prevented hidden connections after closing or switching during startup, including a delayed dynamic module import.
- Packaging: both patches applied cleanly to fresh copies of their exact public baselines; Gitleaks reported no leaks in the export, and a targeted privacy review found no City network identifiers.
- Replay persistence: a token consumed in one container was rejected in a second container using the same named nonce volume and a read-only root filesystem.
- Template: the patched administration template parsed successfully with Template Toolkit.
- Windows build: the public-baseline client plus indicator compiled and linked with Qt 5.5.1 and MinGW 4.9.2 under Wine. This is a compilation check, not a live Windows acceptance test.

The original preview-dialog screenshot is rendered with demonstration data. Two additional screenshots, supplied from the existing deployment, show the staff action buttons (with patron details obscured) and an empty public-computer login screen inside the viewer. Those images document appearance and add no live acceptance-test results.

## Run the focused tests

With the patched server's Perl dependencies installed:

~~~sh
cd libki-server
perl -Ilib ../libkimods/tests/server.t
~~~

For the gateway, from the package directory:

~~~sh
docker build -t libki-viewer:preview gateway
docker run --rm --network none -v "$PWD:/package:ro" --entrypoint python   libki-viewer:preview -m unittest discover -s /package/tests -p test_gateway.py
docker run --rm --network none -v "$PWD:/package:ro" --entrypoint python   libki-viewer:preview /package/tests/run_gateway_runtime.py
~~~

The browser test requires Python Playwright with Chromium installed, a patched server checkout with the installed noVNC files, and a directory containing compatible Bootstrap 4 and jQuery assets named jquery.js, bootstrap.js, and bootstrap-4.0.0.min.css. It intercepts all browser requests; it does not connect to a workstation.

~~~sh
python3 tests/browser_viewer.py ../libki-server /path/to/test-assets
~~~

Rebuild the client using upstream's qmake/make process to repeat compilation.

## Required workstation acceptance before deployment

These checks were **not** performed against a live workstation for this public adaptation:

- Accept a request and verify the expected demonstration screen appears.
- Reject a request, then allow a separate prompt to time out; neither may reveal the screen.
- Repeat with locked, logged-out, and active desktops.
- Attempt keyboard, mouse, clipboard, and file-transfer operations using a separate test VNC client as well as the browser; confirm the endpoint restrictions.
- Verify the firewall rejects direct access from an unauthorized source.
- Confirm the Windows indicator appears/clears as expected; check multi-monitor and secure-desktop behavior against your needs.
- Confirm staff permissions, patron notices, request audit fields, and database behavior in an actual Libki installation.
- Check the deployed reverse proxy's HTTPS/origin handling and verify token-bearing URLs never enter logs.
- Verify nonce persistence after a gateway restart, clock synchronization, shutdown behavior, and rollback.

Keep this package labeled a technical preview until the applicable deployment acceptance checks are complete.
