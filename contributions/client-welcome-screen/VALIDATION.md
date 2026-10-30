# Validation — 2026-10-30

## Passed

- Public baseline: Libki/libki-client cdca236eecce9891715dc1b575cbd01661abd477.
- Source patch applies to a clean export of that baseline; Git whitespace check passes.
- Windows release build: Qt 5.5.1, MinGW 4.9.2, run under Wine in the existing
  development container. No additional client runtime module was introduced.
- Native Qt Test harness: 26 passed, 0 failed (including data-driven cases).
  Results are in tests/test-summary.txt.
- Missing/disabled configuration preserves the upstream layout.
- Invalid intervals fall back or clamp; two-image rotation advances and wraps.
  A five-second timer actually advances; hiding/showing pauses/resumes it.
- Custom library title, resource label and local policy text are read from files.
- Missing images are skipped; the background resizes with the window.
- HTTPS origin boundary cases reject HTTP, file, script, userinfo, wrong port,
  lookalike hostnames and unlisted subdomains.
- The viewer's network manager rejects a file request without returning contents.
- Cookies are rejected and closing clears web history.
- Native username/password fields submit the existing login signal. Login errors
  and server warnings still display; no server receives these test credentials.
- Five screenshots captured from actual rebuilt LoginWindow/WelcomeScreen widgets
  with empty fields and demonstration text. All five visually reviewed.
- Gitleaks directory scan: no leaks found. Targeted text checks found no City
  internal addresses, deployment hostnames, private paths or credential assignments.
- PNG chunk inspection: screenshots contain image/dimension data only; sample
  backgrounds retain their AI-generation C2PA provenance. No private City values
  found in the metadata checks.

## Limits and next acceptance checks

This verifies the portable extraction, not a deployment to public workstations.
The extraction has not been tested against a live Libki/SIP server, real patron
accounts, a physical Windows kiosk or a live external HTTPS information website.
Live redirects/subresources and modern website compatibility were not exercised;
use local HTML for offline information and test each configured website separately.

The build used Libki's legacy Qt 5.5.1 compatibility environment. This is not a
recommendation to expose that old WebKit engine to arbitrary websites. A modern
supported Qt migration is a separate upstream concern. New viewer restrictions
do not apply to upstream's existing logo/banner web views.

Before deployment, verify normal login/logout, reservations, guest registration,
server-provided banners and warnings, focus/keyboard behavior, and the target
screen sizes/DPI with the library's own settings. Recheck after changing fonts,
stylesheets or HTML. Keep the files administrator-controlled.

No production deployment or upstream pull request was performed for this package.
