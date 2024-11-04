//
// Class: YAML
//
// Description: Perform YAML parse/stringify to/from a buffer or
// file. For an in-depth description of the YAML specification refer
// to its RFC at https://yaml.org/spec/1.2.2/.
//
// Dependencies: C++20 - Language standard features used.
//

#include "YAML_Impl.hpp"

namespace YAML_Lib {

YAML::YAML(IStringify *stringify, IParser *parser)
    : implementation(std::make_unique<YAML_Impl>(stringify, parser)) {}

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