#pragma once

// ==========
// YAML Core
// ==========
// 1. Interface definitions (IStringify, IParser, ITranslator, etc.)
#include "YAML_Interfaces.hpp"
// 2. Fundamental base types
#include "YAML_Error.hpp"
#include "YAML_Arena.hpp"
// (YAML_Variant.hpp previously held the Variant base class; now removed.)
// 3. Scalar variant types (depend only on YAML.hpp constants — no Node/container deps)
#include "YAML_Boolean.hpp"
#include "YAML_Comment.hpp"
#include "YAML_Hole.hpp"
#include "YAML_Null.hpp"
#include "YAML_Number.hpp"
#include "YAML_String.hpp"
#include "YAML_Timestamp.hpp"
// 4. Node struct + NodeVariant definition (uses scalar types; forward-declares containers)
#include "YAML_Node.hpp"
// 5. Container variant types (depend on complete Node for vector<Node> members)
#include "YAML_Sequence.hpp"    // SequenceBase<Derived> CRTP base for Array and Document
#include "YAML_Array.hpp"       // struct Array : SequenceBase<Array>
#include "YAML_Dictionary.hpp"  // struct Dictionary, DictionaryEntry (uses Node, String)
#include "YAML_Document.hpp"    // struct Document : SequenceBase<Document>
// 5b. Static-capacity container alternatives (E4 — Profile B/C embedded)
#include "YAML_StaticSequence.hpp"    // StaticSequenceBase<N, Derived> — array-backed CRTP base
#include "YAML_StaticArray.hpp"       // StaticArray<N>, StaticDocument<N>
#include "YAML_StaticDictionary.hpp"  // StaticDictionary<N> — linear-scan, no unordered_map
// 6. Node method implementations (depend on all variant types above)
#include "YAML_Node_Reference.hpp" // isA<T>/NRef<T>; also defines Node::toString/toKey,
                                   // Array::toKey/resize, Dictionary::toKey, Document::resize
#include "YAML_Node_Creation.hpp"  // Node(T) ctors: uses NRef, Boolean, Number, Null, String
#include "YAML_Node_Index.hpp"     // Node::operator[]: uses isA, NRef
// 7. Supporting infrastructure
#include "YAML_Config.hpp"
#include "YAML_Sources.hpp"
#include "YAML_Destinations.hpp"
// 8. Converter
#include "YAML_Converter.hpp"
// 9. Header-only implementations (depend on all of the above)
#include "Default_Translator.hpp"
#include "Default_Parser.hpp"
#include "Default_Stringify.hpp"