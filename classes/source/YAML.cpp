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
unsigned int YAML::getNumberOfDocuments() const { return implementation->getNumberOfDocuments();}
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }
YNode &YAML::root() { return implementation->root(); }
const YNode &YAML::root() const { return implementation->root(); }

} // namespace YAML_Lib