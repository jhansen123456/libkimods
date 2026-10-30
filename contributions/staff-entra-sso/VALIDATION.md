# Validation

This package republishes the unchanged contribution at db04d27fae0814302a0a65594e1ace0662861f28, prepared against upstream 6acf2d1327efcae92fda3509163a9a1da54b99f2.

## Recorded contribution checks

- 84 tests passed across configuration, authorization, account provisioning, controller behavior and OIDC validation.
- OIDC tests exercised the actual dependency offline with ephemeral RSA keys and synthetic metadata, including invalid signature, issuer, audience, nonce, timestamps, authorized party, PKCE and unknown callback state.
- Controller/provisioning tests use test doubles.
- Application startup passed with SSO disabled and with synthetic discovery metadata when enabled.
- The original export passed Gitleaks and targeted private-information checks. The libkimods publication package was scanned again before upload.

Code behavior was not changed for this publication. Prior tests were not rerun solely for packaging.

## Still required

Live sign-in and callback through a real proxy, database session persistence across requests, account/role changes, shared-browser logout, and identity-provider outage recovery remain unverified for this portable export.

Role revocation is not continuous; existing sessions may persist until token expiry or manual invalidation. Recovery accounts remain outside Entra policy. Logout ends the Libki session, not Microsoft's browser session. Enabled startup requires discovery access.

See the [setup guide](docs/staff-entra-sso.md) for the test command and full limitations.
