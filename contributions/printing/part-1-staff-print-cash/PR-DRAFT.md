# [Part 1/2] Review multiple held print jobs and collect cash before release

Staff currently needs a clearer review step when collecting cash for several held jobs. This change adds selection, per-job copy quantities, a combined amount due, cash confirmation and individual release results. Additional reprints use their own review.

Supporting changes validate current prices, claim jobs before submission, and distinguish safe failures from uncertain results. See SCREENSHOTS.md for the workflow with sample data.

Validation: five focused Perl test files passed (47 top-level tests/subtests), plus browser coverage for quantity changes, selection limits, cash reset, reprints, partial failures and repeat submissions. Production printers and payments were not used. Existing API/payment compatibility remains a review item.

Part 2 is planned as a separate companion mobile-print contribution. This part focuses on staff cash collection.

## Contributor context

I am a Network and Systems Administrator in Information Technologies supporting Clinton Public Library in Clinton, Iowa. We have used Libki for about three months to replace EnvisionWare. I am not a programmer, but I developed these changes with Codex and would like to contribute them if useful to the project.

Draft upstream submission text; not sent to maintainers.
