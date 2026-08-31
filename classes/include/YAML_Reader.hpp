#pragma once

/**
 * @file YAML_Reader.hpp
 * @brief Segregated public header for reading and parsing YAML documents.
 *
 * Implements Interface Segregation Principle (ISP) by allowing read-only consumers
 * to include parsing interfaces without pulling in stringifier or conversion targets.
 */

#include "YAML.hpp"
#include "interface/IDOMParser.hpp"
#include "interface/ISAXParser.hpp"
#include "interface/IParser.hpp"
#include "interface/ISource.hpp"
#include "interface/ISchema.hpp"
#ifdef YAML_LIB_FILE_IO
#include "implementation/io/YAML_FileIO.hpp"
#endif
