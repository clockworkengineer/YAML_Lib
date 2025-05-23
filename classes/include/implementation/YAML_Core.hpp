#pragma once

// ==========
// YAML Core
// ==========

// Interfaces
#include "YAML_Interfaces.hpp"

// Structure
#include "YAML_Variant.hpp"
#include "YAML_YNode.hpp"

// Variants
#include "YAML_Hole.hpp"
#include "YAML_Array.hpp"
#include "YAML_Boolean.hpp"
#include "YAML_Comment.hpp"
#include "YAML_Document.hpp"
#include "YAML_Null.hpp"
#include "YAML_Number.hpp"
#include "YAML_String.hpp"
#include "YAML_Dictionary.hpp"

// YNode
#include "YAML_YNode_Reference.hpp"
#include "YAML_YNode_Creation.hpp"
#include "YAML_YNode_Index.hpp"

// IO
#include "YAML_Sources.hpp"
#include "YAML_Destinations.hpp"

// Error
#include "YAML_Error.hpp"

// Defaults
#include "Default_Translator.hpp"
#include "Default_Parser.hpp"
#include "Default_Stringify.hpp"

// Converter
#include "YAML_Converter.hpp"