# Security policy

## Reporting a vulnerability

Please avoid publicly posting exploit details while a security issue is being investigated. Use GitHub's **Report a vulnerability** button on the repository Security tab if private vulnerability reporting is enabled. Otherwise, open a minimal issue asking the maintainer for a private contact method, without including sensitive exploit details.

## Downloads

Download published releases from the official repository. Do not assume source visibility proves a particular binary is safe. Verify the publisher, GitHub Actions build logs, and a checksum if provided. SmartScreen warnings are possible for unsigned binaries.

## Sensitive features

Kova changes Windows display modes and, when enabled, per-display colour settings. It also creates a click-through overlay. These features should be manually tested on supported hardware before using a release, especially in competitive games.
