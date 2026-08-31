# Concrete Documentation Plan for `YAML_Lib`

## Executive Summary

Following the completion of the 100% SOLID architectural refactoring across `YAML_Lib` (Phases 1–4), the project documentation will be expanded and updated. This plan outlines new documentation files to create and existing documentation files to modify to reflect the new segregated interfaces, strategy registries, and extension hooks.

---

## 1. New Documents to Create (`docs/`)

### [`docs/solid_architecture.md`](file:///home/robt/projects/YAML_Lib/docs/solid_architecture.md)
- Architectural deep-dive into how `YAML_Lib` implements all five SOLID design principles.
- Includes component responsibility matrices, system architecture ASCII diagrams, and design trade-offs.

### [`docs/extending_yaml_lib.md`](file:///home/robt/projects/YAML_Lib/docs/extending_yaml_lib.md)
- Step-by-step developer tutorial for extending `YAML_Lib`:
  - Registering custom output formats via `StringifierFactory::instance().registerCreator(...)`.
  - Implementing custom tag/scalar schemas via `ISchema` (`CoreSchema`, `JSONSchema`, `FailsafeSchema`).
  - Implementing custom node allocation strategies via `INodeFactory`.
  - Implementing custom stream sources/destinations with `ISource` / `IDestination`.

---

## 2. Existing Documents to Modify

### [`docs/api.md`](file:///home/robt/projects/YAML_Lib/docs/api.md)
- Add reference sections for `DocumentStore`, `StringifierFactory`, `ISchema`, `CoreSchema`, `IDOMParser`, `ISAXParser`, `INodeFactory`, `DefaultNodeFactory`, `YAML_FileReader`, and `YAML_FileWriter`.

### [`docs/guide.md`](file:///home/robt/projects/YAML_Lib/docs/guide.md)
- Update code snippets to show usage of segregated headers (`YAML_Reader.hpp`, `YAML_Writer.hpp`, `YAML_DOM.hpp`).
- Add code examples for custom schema configuration (`setSchema`) and format registry usage.

### [`docs/public_api.md`](file:///home/robt/projects/YAML_Lib/docs/public_api.md)
- Add `IDOMParser`, `ISAXParser`, `ISchema`, `INodeFactory` to the public interface specification.

### [`README.md`](file:///home/robt/projects/YAML_Lib/README.md)
- Update **Library Design** section to feature segregated header options (`YAML_Reader.hpp`, `YAML_Writer.hpp`, `YAML_DOM.hpp`).
- Add links to `solid_architecture.md` and `extending_yaml_lib.md` in the **Documentation** section.

---

## 3. Implementation Checklist

- [ ] Create `docs/solid_architecture.md`
- [ ] Create `docs/extending_yaml_lib.md`
- [ ] Update `docs/api.md`
- [ ] Update `docs/guide.md`
- [ ] Update `docs/public_api.md`
- [ ] Update `README.md`
