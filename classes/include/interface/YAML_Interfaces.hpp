#pragma once

/**
 * @file YAML_Interfaces.hpp
 * @brief Aggregates all public interface definitions for YAML_Lib.
 *
 * Include this header to access IAction, ISource, IDestination, IParser, IStringify, and ITranslator interfaces.
 *
 * These interfaces form the stable public customization boundary for YAML_Lib.
 * Implementers should preserve the lifetime and ownership semantics documented
 * on each interface while extending parser, stringifier, and I/O behavior.
 */

#include "IAction.hpp"
#include "ISource.hpp"
#include "IDestination.hpp"
#include "IDOMParser.hpp"
#include "ISAXParser.hpp"
#include "IParser.hpp"
#include "IStringify.hpp"
#include "ITranslator.hpp"
#include "ISchema.hpp"