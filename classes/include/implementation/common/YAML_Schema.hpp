#pragma once

// =============================================================================
// E9 — YAML schema validation (Profile B, C)
//
// A lightweight, constexpr-friendly schema descriptor for embedded systems.
// Schema arrays live in ROM; validateAgainst() performs a single pass over a
// parsed top-level Dictionary node — no heap allocation.
//
// Usage:
//   constexpr YAML_Lib::FieldSchema kFields[] = {
//       {"host",    YAML_Lib::NodeType::String,  true},
//       {"port",    YAML_Lib::NodeType::Number,  true},
//       {"debug",   YAML_Lib::NodeType::Boolean, false},
//   };
//   constexpr YAML_Lib::Schema kSchema{kFields, 3};
//
//   auto result = YAML_Lib::validateAgainst(yaml.document(0), kSchema);
//   if (!result.empty()) { /* handle errors */ }
// =============================================================================

namespace YAML_Lib {

// ---------------------------------------------------------------------------
// NodeType enumeration — matches the variant types in NodeVariant.
// ---------------------------------------------------------------------------
enum class NodeType : uint8_t {
  String,
  Number,
  Boolean,
  Null,
  Array,
  Dictionary,
  Timestamp,
  Any,   // wildcard — type check skipped, presence only
};

// ---------------------------------------------------------------------------
// FieldSchema — describes one expected field in a mapping.
// All fields are trivially copyable; the struct is constexpr-constructible
// and can live in ROM.
// ---------------------------------------------------------------------------
struct FieldSchema {
  const char *key;           // null-terminated key name (string literal)
  NodeType    expectedType;  // expected value type; NodeType::Any = any
  bool        required;      // if true, key must be present
};

// ---------------------------------------------------------------------------
// Schema — an array of FieldSchema entries.
// ---------------------------------------------------------------------------
struct Schema {
  const FieldSchema *fields;
  std::size_t        count;
};

// ---------------------------------------------------------------------------
// ValidationError — one problem found during validation.
// message points to a string literal or a short stack string.
// ---------------------------------------------------------------------------
struct ValidationError {
  const char *key;     // which key was the problem (or "" for document-level)
  const char *message; // human-readable description
};

// ---------------------------------------------------------------------------
// validateAgainst — validate a parsed Node against a Schema.
//
// Returns a vector of ValidationError (empty = valid).
// The document node must be a Dictionary; if it is not, one error is returned.
//
// Checks performed:
//   1. For every required field in the schema: key must be present.
//   2. For every field present in the schema: value must match expectedType
//      (unless expectedType == NodeType::Any).
//
// Note: extra keys not mentioned in the schema are not flagged — the library
// remains permissive for forward-compatibility.
// ---------------------------------------------------------------------------
[[nodiscard]] inline std::vector<ValidationError>
validateAgainst(const Node &document, const Schema &schema) {
  std::vector<ValidationError> errors;

  if (!isA<Dictionary>(document)) {
    errors.push_back({"", "Document is not a Dictionary"});
    return errors;
  }

  const auto &dict = NRef<Dictionary>(document);

  for (std::size_t i = 0; i < schema.count; ++i) {
    const FieldSchema &fs = schema.fields[i];

    const bool present = dict.contains(fs.key);

    if (fs.required && !present) {
      errors.push_back({fs.key, "required key missing"});
      continue;
    }

    if (!present) {
      continue; // optional and absent — fine
    }

    if (fs.expectedType == NodeType::Any) {
      continue; // wildcard — skip type check
    }

    const Node &val = document[std::string_view{fs.key}];

    bool typeOk = false;
    switch (fs.expectedType) {
    case NodeType::String:     typeOk = isA<String>(val);     break;
    case NodeType::Number:     typeOk = isA<Number>(val);     break;
    case NodeType::Boolean:    typeOk = isA<Boolean>(val);    break;
    case NodeType::Null:       typeOk = isA<Null>(val);       break;
    case NodeType::Array:      typeOk = isA<Array>(val);      break;
    case NodeType::Dictionary: typeOk = isA<Dictionary>(val); break;
    case NodeType::Timestamp:  typeOk = isA<Timestamp>(val);  break;
    case NodeType::Any:        typeOk = true;                  break;
    }

    if (!typeOk) {
      errors.push_back({fs.key, "value has unexpected type"});
    }
  }

  return errors;
}

} // namespace YAML_Lib
