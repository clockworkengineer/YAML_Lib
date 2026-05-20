# Phase 3 Audit: Reliability and Security

## Changes implemented

- Hardened `Default_Parser::parse()` to enforce `Options::max_documents` for implicit documents created when parsing YAML without an explicit leading `---` marker.
- Also enforced `max_documents` when a document-end marker is encountered with no open document, preventing document-count bypass in edge cases.
- Added a regression test in `tests/source/misc/YAML_Lib_Tests_Options.cpp` to validate that `max_documents = 1` rejects an implicit second document after `---`.

## Verification

- Built `YAML_Lib_Unit_Tests` successfully.
- Ran unit tests filtered to `[YAML][Options][Parse]` and all passed.

## Notes

The parser already included `DepthGuard` for `max_parse_depth` and explicit alias expansion limits in `YAML_Parser_Directive.cpp`, so Phase 3 focused on closing remaining document-count enforcement gaps and documenting the behavior.
