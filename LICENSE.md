# Licensing and attribution

Original additions in this package are copyright 2026 City of Clinton, Iowa.

- **Libki Server patch, gateway, scripts, tests, and documentation:** GNU Affero General Public License, version 3 or later. See [full license](licenses/server-AGPL.txt) and the [upstream server notice](licenses/server-notice.txt).
- **Libki Client patch:** GNU General Public License, version 3 or later. See [full license](licenses/client-GPL.txt). Existing Libki attribution remains in the source.

The patches modify upstream Libki code; existing upstream copyright notices and license obligations continue to apply.

Dependencies are fetched from their public projects rather than copied from the private deployment:

- [noVNC v1.6.0](https://github.com/novnc/noVNC/tree/v1.6.0): predominantly MPL 2.0, with additional file-specific licenses. The installer retains its LICENSE.txt, license documents, and the pako dependency and MIT license.
- [websockify 0.13.0](https://github.com/novnc/websockify/tree/v0.13.0): LGPL 3.0. The Docker build installs its published wheel and license metadata.
- [TightVNC](https://www.tightvnc.com/): separately installed and configured by the operator; no TightVNC executable is distributed here.

Libki's upstream authors and contributors retain their attribution. This package is an independent contribution and does not imply endorsement by the Libki maintainers.
