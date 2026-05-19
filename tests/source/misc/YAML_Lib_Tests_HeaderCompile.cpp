#include "YAML.hpp"
#include "YAML_Core.hpp"

int main() {
  static_assert(sizeof(YAML_Lib::YAML) > 0, "YAML type must be complete");
  static_assert(sizeof(YAML_Lib::Node) > 0, "Node type must be complete");
  return 0;
}
