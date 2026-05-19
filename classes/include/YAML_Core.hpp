

#pragma once


/**
 * @file YAML_Core.hpp
 * @brief Aggregates all core, variant, and container types for advanced YAML_Lib use.
 *
 * Include this header only if you need direct access to node types, containers, or low-level interfaces.
 * Most users should prefer YAML.hpp for the main API.
 *
 * This header pulls in all fundamental types, error macros, interfaces, and containers for advanced YAML manipulation.
 *
 * @section Usage
 *   - Use for direct node manipulation, custom containers, or advanced type access.
 *   - See YAML.hpp for high-level API.
 */

/**
 * @file YAML_Core.hpp
 * @brief Core header aggregating all fundamental types, interfaces, and containers for YAML_Lib.
 *
 * This header should be included to access all core YAML types, error macros, interfaces, and containers.
 * The include order is carefully managed to ensure all dependencies are satisfied for public API use.
 */

// 1. Fundamental error types and macros (needed by all interface headers)
#include "implementation/common/YAML_Error.hpp"
#include "implementation/common/YAML_Arena.hpp"
// 2. Interface definitions (IStringify, IParser, ITranslator, etc.)
//    Must come after YAML_Error.hpp so YAML_MAKE_ERROR is visible.
#include "interface/YAML_Interfaces.hpp"
// (YAML_Variant.hpp previously held the Variant base class; now removed.)
// 3. Scalar variant types (depend only on YAML.hpp constants — no Node/container deps)
#include "implementation/variants/YAML_Boolean.hpp"
#include "implementation/variants/YAML_Comment.hpp"
#include "implementation/variants/YAML_Hole.hpp"
#include "implementation/variants/YAML_Null.hpp"
#include "implementation/variants/YAML_Number.hpp"
#include "implementation/variants/YAML_String.hpp"
#include "implementation/variants/YAML_Timestamp.hpp"
// 4. Node struct + NodeVariant definition (uses scalar types; forward-declares containers)
#include "implementation/node/YAML_Node.hpp"
// 5. Container variant types (depend on complete Node for vector<Node> members)
#include "implementation/variants/YAML_Sequence.hpp"    // SequenceBase<Derived> CRTP base for Array and Document
#include "implementation/variants/YAML_Array.hpp"       // struct Array : SequenceBase<Array>
#include "implementation/variants/YAML_Dictionary.hpp"  // struct Dictionary, DictionaryEntry (uses Node, String)
#include "implementation/variants/YAML_Document.hpp"    // struct Document : SequenceBase<Document>
// 5b. Static-capacity container alternatives (E4 — Profile B/C embedded)
#include "implementation/variants/YAML_StaticSequence.hpp"    // StaticSequenceBase<N, Derived> — array-backed CRTP base
#include "implementation/variants/YAML_StaticArray.hpp"       // StaticArray<N>, StaticDocument<N>
#include "implementation/variants/YAML_StaticDictionary.hpp"  // StaticDictionary<N> — linear-scan, no unordered_map
// 6. Node method implementations (depend on all variant types above)
#include "implementation/node/YAML_Node_Reference.hpp" // isA<T>/NRef<T>; also defines Node::toString/toKey,
                                                      // Array::toKey/resize, Dictionary::toKey, Document::resize
#include "implementation/node/YAML_Node_Creation.hpp"  // Node(T) ctors: uses NRef, Boolean, Number, Null, String
#include "implementation/node/YAML_Node_Index.hpp"     // Node::operator[]: uses isA, NRef
// 7. Supporting infrastructure
#include "YAML_Config.hpp"
#include "implementation/io/YAML_Sources.hpp"
#include "implementation/io/YAML_Destinations.hpp"
// 7b. E9: Schema validation (depends on isA/NRef/operator[] defined above)
#include "implementation/common/YAML_Schema.hpp"
// 7c. E10: SAX-style event API (depends on NodeType from YAML_Schema.hpp)
#include "implementation/common/YAML_SAX.hpp"
// 8. Converter
#include "implementation/converter/YAML_Converter.hpp"
// 9. Header-only implementations (depend on all of the above)
#include "implementation/translator/Default_Translator.hpp"
#include "implementation/parser/Default_Parser.hpp"
#include "implementation/stringify/Default_Stringify.hpp"