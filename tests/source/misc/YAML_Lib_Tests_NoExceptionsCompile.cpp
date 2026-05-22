#include "YAML.hpp"
#include "YAML_Core.hpp"

namespace YAML_Lib {
namespace {
void customPanicHandler(std::string_view, unsigned long, unsigned long) noexcept {}
} // namespace
} // namespace YAML_Lib

int main() {
  YAML_Lib::setErrorHandler(YAML_Lib::customPanicHandler);
  return YAML_Lib::getErrorHandler() == YAML_Lib::customPanicHandler ? 0 : 1;
}
