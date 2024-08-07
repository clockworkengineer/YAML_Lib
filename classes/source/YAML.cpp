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

/// <summary>
/// YAML destructor.
/// </summary>
YAML::~YAML() {}

std::string YAML::version() const { return (YAML_Impl::version()); }
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }

} // namespace YAML_Lib