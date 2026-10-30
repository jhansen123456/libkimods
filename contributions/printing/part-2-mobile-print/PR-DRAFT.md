# [Part 2/2] Companion mobile printing with guest uploads and per-file copies

This companion service allows guest uploads without a library card, incremental selection of up to five documents, removal and per-file quantities. It prepares a PDF bundle and gives the patron a release code, price breakdown and pickup instructions before staff collects cash and releases the job.

Libki already supports web printing. This proposal contributes our particular guest upload, document-processing and staff-pickup workflow. It builds on the staff review workflow proposed in Part 1.

The package includes sanitized source and five screenshots using fabricated data. It excludes deployment secrets and City network configuration. Generic deployment instructions and upstream intake compatibility are still in preparation; this is not advertised as ready to install.

Validation: 12 focused Python tests passed; one real-qpdf fixture test was skipped. Browser checks covered incremental selection, quantities, removal, limits, escaping and phone layout. No end-to-end production printing was performed.

## Contributor context

I am a Network and Systems Administrator in Information Technologies supporting Clinton Public Library in Clinton, Iowa. We have used Libki for about three months to replace EnvisionWare. I am not a programmer, but I developed these changes with Codex and would like to contribute them if useful to the project.

Draft upstream submission text; not sent to maintainers. Submit after the Part 1 contribution is published and Part 2's integration prerequisites are addressed.
