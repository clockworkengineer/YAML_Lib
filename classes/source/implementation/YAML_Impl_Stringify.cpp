
#include <algorithm>
#include "YAML_Impl.hpp"

namespace YAML_Lib {

/// <summary>
/// Function header.
/// </summary>
void YAML_Impl::stringify(IDestination &destination) const {
  destination.reserve(std::max<std::size_t>(4096, documentStore.size() * 512));
  for (auto &document : documentStore.getDocuments()) {
    yamlStringify->stringify(document, destination, 0);
  }
}

} // namespace YAML_Lib
