# Install the screen-viewer preview

Use an isolated Libki evaluation environment first. The feature is disabled by default and supports one explicitly configured workstation per deployment. This guide adds it to an existing Libki installation; it does not install or configure the entire Libki stack.

## 1. Apply the source patches

These are the exact public upstream baselines used for this package:

| Project | Baseline commit |
| --- | --- |
| Libki Server | 6acf2d1327efcae92fda3509163a9a1da54b99f2 |
| Libki Client | cdca236eecce9891715dc1b575cbd01661abd477 |

From a new working directory:

~~~sh
git clone https://github.com/jhansen123456/libkimods.git
git clone https://github.com/Libki/libki-server.git
git -C libki-server checkout -b viewer-preview 6acf2d1327efcae92fda3509163a9a1da54b99f2
git -C libki-server apply --check ../libkimods/patches/server-viewer.patch
git -C libki-server apply ../libkimods/patches/server-viewer.patch
python3 libkimods/scripts/install_novnc.py libki-server
~~~

The installer downloads noVNC v1.6.0 at a fixed commit, checks the archive SHA-256, and installs its core, pako dependency, and license texts. It refuses to replace an existing noVNC directory. Rebuild your Libki server image/application from this patched checkout using the upstream build instructions.

The Windows indicator is optional and separate:

~~~sh
git clone https://github.com/Libki/libki-client.git
git -C libki-client checkout -b viewer-indicator-preview cdca236eecce9891715dc1b575cbd01661abd477
git -C libki-client apply --check ../libkimods/patches/client-indicator.patch
git -C libki-client apply ../libkimods/patches/client-indicator.patch
~~~

Build the client using its upstream Qt build process. This export compiled and linked with Qt 5.5.1 and MinGW 4.9.2 under Wine; no installer or binary is published here. That compatibility check is not a recommendation to deploy an old Qt runtime.

## 2. Configure workstation enforcement

Install and administer the VNC server separately. The original design targets TightVNC Server for Windows. Settings and defaults vary by release; use the installed version's documentation and test its behavior.

Required configuration:

- Use a dedicated VNC port, such as 5901, and a separate **view-only** password. Never put a control/admin password in the Libki password file.
- Block remote keyboard and mouse input at the VNC server (TightVNC's BlockRemoteInput setting).
- Disable file transfers (EnableFileTransfers) and the HTTP/Java viewer (AcceptHttpConnections). Disable clipboard synchronization where the installed server offers that setting; the browser has no clipboard UI.
- In access control, give the gateway source address the action **Query local user**. A query timeout alone does not enable prompting.
- Set the timeout action to **reject** and test both an explicit rejection and no response. The original configuration used a 30-second timeout.
- Restrict the workstation firewall to the gateway's actual source address and this port. Remove broader VNC allow rules. Do not expose this listener to the Internet.
- Verify prompting and rejection on locked/logged-out screens as well as an active desktop. Keep the feature disabled if any case can bypass the required local approval.

TightVNC's [official 2.7 installation reference](https://www.tightvnc.com/doc/win/TightVNC_2.7_for_Windows_Installing_from_MSI_Packages.pdf) explains query rules and timeout settings; it is historical documentation, not confirmation of the defaults in a newer installed version.

The Libki notice and audit record do **not** configure or verify these endpoint controls.

## 3. Create secrets outside Git

Create a dedicated deployment directory outside this repository. The example generates a shared HMAC key and an 8-character VNC view-only password. Set that password as the workstation's view-only credential through its supported configuration interface.

~~~sh
umask 077
mkdir -p secrets
python3 - <<'PY'
from pathlib import Path
import secrets, string
Path("secrets/viewer_hmac").write_text(secrets.token_hex(32))
alphabet = string.ascii_letters + string.digits
Path("secrets/viewer_password").write_text("".join(secrets.choice(alphabet) for _ in range(8)))
PY
~~~

Mount the HMAC file read-only into both Libki and the gateway. Mount the VNC password file read-only into Libki only. The service users must be able to read their files; keep permissions scoped to those users. The gateway runs as UID/GID 65534, so adjust file ownership or ACLs accordingly. Local Compose bind-backed secrets may not honor a declared UID/GID override.

The short VNC password limit reflects classic VNC authentication. It makes network isolation and encrypted transport especially important.

## 4. Configure Libki

Use [viewer.env.example](../examples/viewer.env.example) as a list of settings:

| Setting | Purpose |
| --- | --- |
| LIBKI_VIEWER_ENABLED | Set to true only after completing endpoint configuration |
| LIBKI_VIEWER_ORIGIN | Exact browser HTTPS origin, such as https://libki.example.org; no trailing slash/path |
| LIBKI_VIEWER_INSTANCE | Exact allowed Libki instance; empty selects the default instance |
| LIBKI_VIEWER_CLIENT_NAME | Exact existing Libki client name |
| LIBKI_VIEWER_HMAC_FILE | Mounted shared signing-key file |
| LIBKI_VIEWER_PASSWORD_FILE | Mounted workstation view-only-password file |

Do not load the environment example unchanged into production. Its hostname and 192.0.2.10 address are documentation examples.

The configured workstation belongs to exactly one Libki instance. Requests from other instances are rejected even if they contain the same workstation name. Use a dedicated signing key for this gateway/instance mapping.

The token endpoint requires the browser's Origin header to match the configured origin and its X-Requested-With header to equal XMLHttpRequest. Do not enable permissive CORS on the endpoint. The patched UI sends these requests automatically.

## 5. Build and connect the gateway

~~~sh
docker build -t libki-viewer:preview libkimods/gateway
~~~

[compose.gateway.yml](../examples/compose.gateway.yml) is a supplementary example. Adapt it to your existing reverse proxy and secret-file layout. Its relative build/secret paths are resolved against the Compose file's directory; keep actual secrets outside Git.

Set LIBKI_VIEWER_CLIENT_NAME to the same Libki name, LIBKI_VIEWER_TARGET_HOST to the workstation IPv4 address, and LIBKI_VIEWER_TARGET_PORT to its VNC port. The browser cannot select an arbitrary destination.

The gateway listens on container port 6080. Publish no host port. Connect only the trusted reverse proxy to its proxy network, and restrict gateway egress to the intended VNC target using your host/network firewall.

Keep the named nonce volume across restarts. A private writable nonce directory shared by the gateway's worker processes provides atomic replay protection. Removing it discards recently consumed tokens; rotate the HMAC key or wait for all issued tokens to expire before restarting with empty state. This first release assumes a single gateway instance and local filesystem semantics.

Apply [nginx-viewer.conf](../examples/nginx-viewer.conf) inside your existing HTTPS virtual host, replace the example origin, and connect its upstream to the gateway. Apply your staff-access policy to the route. Turn off token-bearing URL logging throughout the proxy chain, including external gateways/CDNs. The supplied gateway entry point suppresses websockify logging to prevent raw tokens appearing in error logs; monitor process health and Libki's request audit separately.

Use HTTPS/WSS for the browser connection. The gateway-to-workstation RFB connection is not encrypted by this package: protect that hop with an authenticated tunnel or an appropriately isolated trusted network.

## 6. Enable the optional Windows indicator

Add to the Libki client's normal settings file:

~~~ini
[viewer_indicator]
enabled=true
local_port=5901
~~~

The port must match the workstation VNC listener. The indicator defaults to disabled. It checks established IPv4 TCP connections every 750 ms, appears on the primary screen, and does not implement a consent or access-control mechanism. Non-Windows builds do not display an active-connection indication.

## 7. Validate, then enable

Complete [VALIDATION.md](VALIDATION.md) on a demonstration workstation. Retain your original application images/configuration for rollback.

To stop new browser requests, set LIBKI_VIEWER_ENABLED=false and restart the Libki application. That does not terminate an already established gateway/VNC connection. To stop active viewing, close viewers and stop the dedicated gateway or workstation VNC service according to your normal operational procedure. Restore the original application build if removing the patches.
