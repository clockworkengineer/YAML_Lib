#pragma once

#include <vector>
#include "implementation/common/YAML_Error.hpp"

namespace YAML_Lib {

class ISource;
struct Node;

/**
 * @brief Segregated interface for YAML DOM parsing.
 *
 * Implements Interface Segregation Principle (ISP) by allowing clients
 * to depend only on DOM node vector output without pulling in SAX dependencies.
 */
class IDOMParser {
public:
  YAML_MAKE_ERROR(Error, "IDOMParser Error");
  virtual ~IDOMParser() = default;

  [[nodiscard]] virtual std::vector<Node> parse(ISource &source) = 0;
};

} // namespace YAML_Lib
