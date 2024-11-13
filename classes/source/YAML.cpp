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

/// <summary>
/// 
/// </summary>
/// <param name="stringify"></param>
/// <param name="parser"></param>
YAML::YAML(IStringify *stringify, IParser *parser)
    : implementation(std::make_unique<YAML_Impl>(stringify, parser)) {}
/// <summary>
/// 
/// </summary>
YAML::~YAML() = default;
/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::string YAML::version() { return (YAML_Impl::version()); }
unsigned int YAML::getNumberOfDocuments() const {
  return implementation->getNumberOfDocuments();
}
/// <summary>
/// 
/// </summary>
/// <param name="source"></param>
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }
/// <summary>
/// 
/// </summary>
/// <param name="destination"></param>
void YAML::stringify(IDestination &destination) const {
  implementation->stringify(destination);
}
void YAML::stringify(IDestination &&destination) const {
  implementation->stringify(destination);
}
/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::vector<YNode> &YAML::root() { return implementation->root(); }
const std::vector<YNode> &YAML::root() const { return implementation->root(); }
/// <summary>
/// 
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
YNode &YAML::document(unsigned long index) {
  return implementation->document(index);
}
const YNode &YAML::document(unsigned long index) const {
  return implementation->document(index);
}

} // namespace YAML_Lib