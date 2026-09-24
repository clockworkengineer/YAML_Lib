// Isolated translation unit verifying interface headers compile standalone
#include "interface/ISource.hpp"
#include "interface/IDestination.hpp"
#include "interface/IAction.hpp"
#include "interface/IDOMParser.hpp"
#include "interface/ISAXParser.hpp"
#include "interface/IParser.hpp"
#include "interface/IStringify.hpp"
#include "interface/ITranslator.hpp"
#include "interface/ISchema.hpp"
#include "interface/INodeFactory.hpp"
#include "interface/YAML_Interfaces.hpp"

namespace {
struct TestCustomSource : YAML_Lib::ISource {
  char current() const override { return '\0'; }
  void next() override {}
  bool more() const override { return false; }
  void reset() override {}
  std::size_t position() override { return 0; }
  void save() override {}
  void restore() override {}
  void discardSave() override {}
protected:
  void backup(unsigned long) override {}
};

struct TestCustomDest : YAML_Lib::IDestination {
  void add(char) override {}
  void clear() override {}
  char last() override { return '\0'; }
};
}

void testCompileInterfaces() {
  TestCustomSource src;
  TestCustomDest dst;
  (void)src;
  (void)dst;
}
