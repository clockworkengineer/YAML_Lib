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

YAML::~YAML() {}

std::string YAML::version() const { return (YAML_Impl::version()); }
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
std::vector<YNode> &YAML::document() { return implementation->document(); }
const std::vector<YNode> &YAML::document() const { return implementation->document(); }

} // namespace YAML_Lib