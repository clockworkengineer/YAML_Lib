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
/// Create YAML object.
/// </summary>
/// <param name="stringify">Pointer to stringifier interface.</param>
/// <param name="parser">Pointer to parser interface.</param>
YAML::YAML([[maybe_unused]]IStringify *stringify, [[maybe_unused]]IParser *parser)
    : implementation(std::make_unique<YAML_Impl>(stringify, parser)) {}
/// <summary>
/// Destroy YAML object.
/// </summary>
YAML::~YAML() = default;
/// <summary>
/// Fetch version string for current YAML_Lib.
/// </summary>
/// <returns></returns>
std::string YAML::version() { return YAML_Impl::version(); }
unsigned int YAML::getNumberOfDocuments() const {
  return implementation->getNumberOfDocuments();
}
/// <summary>
/// Parse YAML from source stream into the YNode tree.
/// </summary>
/// <param name="source"></param>
void YAML::parse(ISource &source) const { implementation->parse(source); }
void YAML::parse(ISource &&source) const { implementation->parse(source); }
/// <summary>
/// Stringify YNode tree to destination stream (file/buffer/network).
/// </summary>
/// <param name="destination"></param>
void YAML::stringify(IDestination &destination) const {
  implementation->stringify(destination);
}
void YAML::stringify(IDestination &&destination) const {
  implementation->stringify(destination);
}
/// <summary>
/// Return root of YNode tree.
/// </summary>
/// <returns></returns>
std::vector<YNode> &YAML::root() { return implementation->root(); }
const std::vector<YNode> &YAML::root() const { return implementation->root(); }
/// <summary>
/// Return YNode of the index document within YAML tree.
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
YNode &YAML::document(const unsigned long index) {
  return implementation->document(index);
}
const YNode &YAML::document(const unsigned long index) const {
  return implementation->document(index);
}

} // namespace YAML_Lib