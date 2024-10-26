//
// Class: YAML
//
// Description:
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YAML::YAML() : implementation(std::make_unique<YAML_Impl>()) {}

YAML::~YAML() = default;

std::string YAML::version() { return (YAML_Impl::version()); }
unsigned int YAML::getNumberOfDocuments() const {
  return implementation->getNumberOfDocuments();
}
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }
void YAML::stringify(IDestination &destination) const {
  implementation->stringify(destination);
}
void YAML::stringify(IDestination &&destination) const {
  implementation->stringify(destination);
}
std::vector<YNode> &YAML::root() { return implementation->root(); }
const std::vector<YNode> &YAML::root() const { return implementation->root(); }
YNode &YAML::document(unsigned long index) {
  return implementation->document(index);
}
const YNode &YAML::document(unsigned long index) const {
  return implementation->document(index);
}

} // namespace YAML_Lib