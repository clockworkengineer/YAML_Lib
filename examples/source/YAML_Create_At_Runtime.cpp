//
// Program: YAML_Create_At_Runtime
//
// Description: Create YAML programmatically at runtime using a combination
// of the YAML class, indexing/key operators plus the use of initializer lists.
//
// Dependencies: C++20, PLOG, YAML_Lib.
//

#include "YAML_Utility.hpp"

namespace yl = YAML_Lib;
namespace fs = std::filesystem;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
  try {
    // Initialise logging.
    plog::init(plog::debug, "YAML_Create_At_Runtime.log");
    PLOG_INFO << "YAML_Create_At_Runtime started ...";
    // Log version
    PLOG_INFO << yl::YAML().version();
    // create an empty structure (null)
    yl::YAML yaml;
    // add a number that is stored as double (note the implicit conversion of
    // yaml to an object)
    yaml["pi"] = 3.141;
    // add a Boolean that is stored as bool
    yaml["sad"] = true;
    // add a string that is stored as std::string
    yaml["first_name"] = "Niels";
    // add another null object by passing nullptr
    yaml["nothing"] = nullptr;
    // add an object inside the object
    yaml["the_answer"]["everything"] = 42;
    // add an array that is stored as std::vector (using an initializer list)
    yaml["list"] = { 1, 0, 2 };
    // add another object (using an initializer list of pairs)
    yaml["object"] = { { "currency", "USD" }, { "value", 42.99 } };
    // add another object that has a nested array (YNode{})
    yaml["object2"] = { { "currency", "USD" }, { "array", yl::YNode{ 23.22, 33, 55, 99.99 } } };
    yl::BufferDestination destination;
    yaml.stringify(destination);
    PLOG_INFO << destination.toString();
    destination.clear();
    // create YAML using an initializer list and nesting array/objects using YNode{}.
    yl::YAML yaml2 = { { "pi", 3.141 },
      { "sad", true },
      { "first_name", "Niels" },
      { "nothing", nullptr },
      { "the_answer", yl::YNode{ { "everything", 42 } } },
      { "list", yl::YNode{ 1, 0, 2 } },
      { "object", yl::YNode{ { "currency", "USD" }, { "value", yl::YNode{ 23.22, 33, 55, 99.99 } } } } };
    yaml2.stringify(destination);
    PLOG_INFO << destination.toString();
  } catch (std::exception &ex) {
    PLOG_ERROR << "Error: " << ex.what();
  }
  PLOG_INFO << "YAML_Create_At_Runtime exited.";
  exit(EXIT_SUCCESS);
}