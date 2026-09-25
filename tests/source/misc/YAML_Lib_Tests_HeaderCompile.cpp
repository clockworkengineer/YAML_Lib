#include "YAML.hpp"
#include "YAML_Core.hpp"
#include "YAML_Reader.hpp"
#include "YAML_Writer.hpp"
#include "YAML_DOM.hpp"

void testCompileInterfaces();

int main() {
  testCompileInterfaces();
  static_assert(sizeof(YAML_Lib::YAML) > 0, "YAML type must be complete");
  static_assert(sizeof(YAML_Lib::Node) > 0, "Node type must be complete");
  return 0;
}
