#pragma once

// ==========
// YAML Core
// ==========
// 1. Interface definitions (IStringify, IParser, ITranslator, etc.)
#include "YAML_Interfaces.hpp"
// 2. Fundamental base types
#include "YAML_Error.hpp"
#include "YAML_Variant.hpp"
// 3. Node struct declaration (depends on Variant; YAML::ArrayInitializer from YAML.hpp)
#include "YAML_Node.hpp"
// 4. Primitive variant types (depend only on Variant — no cross-variant or Node::make deps)
#include "YAML_Boolean.hpp"
#include "YAML_Comment.hpp"
#include "YAML_Hole.hpp"
#include "YAML_Null.hpp"
#include "YAML_Number.hpp"
#include "YAML_String.hpp"
#include "YAML_Timestamp.hpp"
// 5. Container variant types (depend on Node + specific primitives above)
#include "YAML_Array.hpp"       // uses Node::make<Hole>
#include "YAML_Dictionary.hpp"  // uses Node::make<String>
#include "YAML_Document.hpp"    // uses Node::make<Hole>
// 6. Node method implementations (depend on all variant types above)
#include "YAML_Node_Reference.hpp" // defines isA<T>/NRef<T>: uses all variant types
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