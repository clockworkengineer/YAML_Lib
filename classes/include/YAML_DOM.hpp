#pragma once

/**
 * @file YAML_DOM.hpp
 * @brief Segregated public header for YAML Document Object Model (DOM) manipulation.
 *
 * Implements Interface Segregation Principle (ISP) by exposing node, variant,
 * and DocumentStore container types for tree traversal and modification.
 */

#include "YAML_Core.hpp"
#include "implementation/common/YAML_DocumentStore.hpp"
