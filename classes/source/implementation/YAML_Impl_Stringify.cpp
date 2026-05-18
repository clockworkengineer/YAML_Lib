
#include <algorithm>
#include "YAML_Impl.hpp"

namespace YAML_Lib {

void YAML_Impl::stringify(IDestination &destination) const {
  destination.reserve(std::max<std::size_t>(4096, yamlTree.size() * 512));
  for (auto &document : yamlTree) {
    yamlStringify->stringify(document, destination, 0);
  }
}

} // namespace YAML_Lib
