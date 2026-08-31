#pragma once

/**
 * @file YAML_Writer.hpp
 * @brief Segregated public header for formatting and writing YAML documents.
 *
 * Implements Interface Segregation Principle (ISP) by allowing write-only consumers
 * to include stringifier interfaces without pulling in parser or lexer dependencies.
 */

#include "YAML.hpp"
#include "interface/IStringify.hpp"
#include "interface/IDestination.hpp"
#include "implementation/common/YAML_StringifierFactory.hpp"
#ifdef YAML_LIB_FILE_IO
#include "implementation/io/YAML_FileIO.hpp"
#endif
