#pragma once

#include "implementation/common/YAML_Error.hpp"

namespace YAML_Lib {

class ISource;
class IYAMLEvents;

/**
 * @brief Segregated interface for push-based SAX event parsing.
 *
 * Implements Interface Segregation Principle (ISP) by allowing clients
 * to depend only on event-driven streaming without instantiating DOM trees.
 */
class ISAXParser {
public:
  YAML_MAKE_ERROR(Error, "ISAXParser Error");
  virtual ~ISAXParser() = default;

  virtual void parseSAX(ISource &source, IYAMLEvents &events) = 0;
};

} // namespace YAML_Lib
