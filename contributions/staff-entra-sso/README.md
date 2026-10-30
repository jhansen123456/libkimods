# Staff Microsoft Entra authentication and single sign-on

Optional Microsoft sign-in for Libki staff. With the feature enabled, opening staff administration starts Microsoft sign-in; an existing Microsoft browser session can provide single sign-on, subject to tenant policy. Patron login is unchanged.

**Status: draft contribution for technical review.** This package mirrors [upstream draft pull request #507](https://github.com/Libki/libki-server/pull/507) at commit db04d27fae0814302a0a65594e1ace0662861f28. It is not an official Libki release. The portable export still needs live Entra acceptance testing.

## What is included

- Single-tenant authorization-code authentication with PKCE.
- Explicit staff and administrator app-role assignments, with optional group mapping.
- Entra-owned Libki accounts and role reconciliation at sign-in.
- Local recovery login, session rotation, token cleanup, and session expiry.
- A signed-out page and generic provider errors.
- Configuration examples and five automated test files.

This is a server contribution; it does not require the screen viewer, printing contributions, or Libki Client changes.

## Review and installation

- [Setup, app registration, configuration, recovery and security boundaries](docs/staff-entra-sso.md)
- [Complete source patch](staff-entra-sso.patch)
- [Changed source files and tests](changes/)
- [Validation and remaining acceptance work](VALIDATION.md)

Apply the patch to a separate checkout of the exact upstream baseline:

~~~sh
git clone https://github.com/Libki/libki-server.git
cd libki-server
git switch -c review/staff-entra-sso 6acf2d1327efcae92fda3509163a9a1da54b99f2
git apply --check /path/to/staff-entra-sso.patch
git apply /path/to/staff-entra-sso.patch
~~~

Replace the patch path with your downloaded file. Install dependencies using Libki's cpanfile or Makefile.PL workflow, then follow the setup guide. This package contains changed files, not a complete standalone server. The feature is disabled by default; each library must supply its own Entra registration and secret.

## Contribution background

Developed for Clinton Public Library in Clinton, Iowa, while using Libki to replace EnvisionWare. Josh Hansen is a Network and Systems Administrator in Information Technologies, not a programmer, and developed these modifications with help from Codex.

Only the selected contribution files are included. No private Git history, tenant configuration, credentials, staff records, database contents, or City network settings are included. Examples and test identities are synthetic.

See [the upstream license notice](LICENSE) and [AGPL license text](../../licenses/server-AGPL.txt).
